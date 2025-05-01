#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QScrollArea>
#include <QFrame>
#include <QTimer>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <mutex>
#include <QThread>

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QPushButton>

#include "listWidget.h"
#include "grannt.h"
#include "processinputwidget.h"
#include "header.h"

using namespace std;

class SchedulerWorker : public QObject {
    Q_OBJECT
public:
    scheduler* sched;
    algorithm* algo;

public slots:
    void run() {
        if (sched && algo) {
            sched->inProgress(*algo);
            emit finished();
        }
    }

signals:
    void finished();
};
bool livePreview = true;
bool finished = false; 
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(std::vector<process*>& processes, std::mutex& mutexRef, scheduler* sched, algorithm* algo, QWidget* parent = nullptr)
        : QMainWindow(parent), processes(processes), processMutex(mutexRef), currentTime(0), sched(sched), algo(algo) {

        setWindowTitle("Process Scheduling OS Team");
        resize(1200, 800);

        QWidget* centralWidget = new QWidget();
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

        QWidget* contentWidget = new QWidget();
        QHBoxLayout* contentLayout = new QHBoxLayout(contentWidget);

        processListWidget = new ProcessListWidget();
        processListWidget->setMinimumWidth(300);
        contentLayout->addWidget(processListWidget);

        QWidget* rightPanel = new QWidget();
        QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

        ganttChartWidget = new GanttChartWidget();
        rightLayout->addWidget(ganttChartWidget, 1);
        
        QWidget* metricsPanel = new QWidget();
        metricsPanel->setStyleSheet("background-color: #808080; border-radius: 5px;");
        QHBoxLayout* metricsLayout = new QHBoxLayout(metricsPanel);

        // Average Waiting Time
        avgWaitingLabel = new QLabel("Avg Waiting: --");
        avgWaitingLabel->setAlignment(Qt::AlignCenter);
        avgWaitingLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        metricsLayout->addWidget(avgWaitingLabel);

        // Average Turnaround Time
        avgTurnaroundLabel = new QLabel("Avg Turnaround: --");
        avgTurnaroundLabel->setAlignment(Qt::AlignCenter);
        avgTurnaroundLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        metricsLayout->addWidget(avgTurnaroundLabel);

        rightLayout->addWidget(metricsPanel);

        inputWidget = new ProcessInputWidget();
        rightLayout->addWidget(inputWidget);

        contentLayout->addWidget(rightPanel, 1);
        mainLayout->addWidget(contentWidget, 1);

        setCentralWidget(centralWidget);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);

        connect(inputWidget, &ProcessInputWidget::addProcessClicked, this, &MainWindow::addProcess);
        connect(inputWidget, &ProcessInputWidget::runSchedulerClicked, this, &MainWindow::runScheduler);

        updateUI();
    }

    void updateUI() {
        // Get snapshot of processes safely
        auto processSnapshot = getProcessSnapshot();

        // Update widgets with the snapshot
        QMetaObject::invokeMethod(this, [this, processSnapshot]() {
            ganttChartWidget->updateGanttChart(processSnapshot, currentTime);
                processListWidget->updateProcessList(processSnapshot);
        }, Qt::QueuedConnection);
        ganttChartWidget->updateTimeCounter(currentTime);
        updatePerformanceMetrics();

    }

    void updatePerformanceMetrics() {
        std::lock_guard<std::mutex> lock(processMutex);

        // Calculate and update waiting time
        if (finished) {
            float avgTurnaround = sched->avgTurnAround();
            avgTurnaroundLabel->setText(QString("Avg Turnaround: %1").arg(avgTurnaround, 0, 'f', 2));
            float avgWait = sched->avgWaiting();
            avgWaitingLabel->setText(QString("Avg Waiting: %1").arg(avgWait, 0, 'f', 2));

            // Calculate and update turnaround time
            
            /*if (avgWait > 10.0) {
                avgWaitingLabel->setStyleSheet("color: red; font-weight: bold;");
            }
            else {
                avgWaitingLabel->setStyleSheet("color: green; font-weight: bold;");
            }*/
        }else {
			avgWaitingLabel->setText("Avg Waiting: --");
			avgTurnaroundLabel->setText("Avg Turnaround: --");
        }
        // Optional: Color coding based on performance
        
    }
private slots:
    std::vector<process*> getProcessSnapshot() {
        std::vector<process*> snapshot;

        // Use scheduler's mutex to access processes
        std::lock_guard<std::mutex> lock(sched->processMutex);
        snapshot = processes; // Copy the vector

        return snapshot;
    }

    void updateTimer() {
        // Calculate max time
        auto processSnapshot = getProcessSnapshot();
         scheduler * sched = scheduler::getInstance();
          
    
        float maxTime = 0;
        for (const auto& process : processSnapshot) {
			/*maxTime += process->getBurstTime();*/
            maxTime = max(maxTime, process->getArrivalTime()); 
			if (process->getFinished()) {
				maxTime = max(maxTime, (float) (process->getTurnAroundTime()+process->getArrivalTime()));
			}
            if (process->getRemainingTime() > 0 )
			maxTime = max(maxTime, SingletonCounter::getInstance().getValue() + process->getRemainingTime());
        }
        currentTime = livePreview ? SingletonCounter::getInstance().increment() : maxTime;

        // Get snapshot safely
        updateUI();

        if (currentTime > maxTime+5) {
            timer->stop();
            finished = true; 
            updatePerformanceMetrics();
        }
    }

    

    void addProcess() {
        process* p = new process(inputWidget->getCurrentProcess());
        // Use scheduler's method which has its own mutex protection
        sched->addProcess(p);
        //QDebug::QDebug() << "Added process:" << p->getName() << "with arrival time:" << p->getArrivalTime();

        updateUI();
    }
    void runScheduler() {

        // Start the thread
        QThread* thread = new QThread;
        SchedulerWorker* worker = new SchedulerWorker;
        worker->sched = sched;
        worker->algo = algo;

        worker->moveToThread(thread);
        connect(thread, &QThread::started, worker, &SchedulerWorker::run);
        connect(worker, &SchedulerWorker::finished, thread, &QThread::quit);
        connect(worker, &SchedulerWorker::finished, worker, &SchedulerWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
        timer->start(1000);
    }

private:
    std::vector<process*>& processes;
    std::mutex& processMutex;
    scheduler* sched;
    algorithm* algo;
    ProcessListWidget* processListWidget;
    GanttChartWidget* ganttChartWidget;
    ProcessInputWidget* inputWidget;
    QLabel* avgWaitingLabel;
    QLabel* avgTurnaroundLabel;
    QTimer* timer;
    float currentTime;
};
#include <QGroupBox>
#include <QRadioButton>
#include <qmessagebox.h> 
#include <QTableWidget>
class SetupDialog : public QDialog {
    Q_OBJECT
public:
    explicit  SetupDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Simulation Setup");
        setFixedSize(600, 600);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        // Algorithm Selection
        QGroupBox* algoGroup = new QGroupBox("Scheduling Algorithm");
        QVBoxLayout* algoLayout = new QVBoxLayout();

        algorithmCombo = new QComboBox();
        algorithmCombo->addItems({ "First-Come First-Served (FCFS)",
                                "Shortest Job First (SJF)",
                                "Priority Scheduling",
                                "Round Robin" });
        algoLayout->addWidget(algorithmCombo);

        // Preemption Option
        preemptiveCheck = new QCheckBox("Preemptive Mode");
        preemptiveCheck->setChecked(true);
        algoLayout->addWidget(preemptiveCheck);

        // Time Quantum (for Round Robin)
        quantumGroup = new QGroupBox("Time Quantum (for Round Robin)");
        QHBoxLayout* quantumLayout = new QHBoxLayout();
        quantumSpin = new QSpinBox();
        quantumSpin->setRange(1, 20);
        quantumSpin->setValue(4);
        quantumLayout->addWidget(quantumSpin);
        quantumGroup->setLayout(quantumLayout);
        quantumGroup->setVisible(false);
        algoLayout->addWidget(quantumGroup);

        algoGroup->setLayout(algoLayout);
        mainLayout->addWidget(algoGroup);

        // Connect algorithm changes
        connect(algorithmCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) {
                // Show quantum only for Round Robin
                quantumGroup->setVisible(index == 3);

                // Enable preemptive only for SJF and Priority
                bool enablePreemptive = (index == 1 || index == 2);
                preemptiveCheck->setEnabled(enablePreemptive);

                // Enable priority only for Priority Scheduling
                bool enablePriority = (index == 2);  // Priority Scheduling is index 2
                prioritySpin->setEnabled(enablePriority);

                // Uncheck if disabled
                if (!enablePreemptive) {
                    preemptiveCheck->setChecked(false);
                }

                // Set tooltips
                if (index == 0) { // FCFS
                    preemptiveCheck->setToolTip("FCFS is always non-preemptive");
                }
                else if (index == 3) { // Round Robin
                    preemptiveCheck->setToolTip("Round Robin is always preemptive (time quantum controlled)");
                }
                else {
                    preemptiveCheck->setToolTip("");
                }

                prioritySpin->setToolTip(enablePriority ?
                    "Set process priority (lower numbers = higher priority)" :
                    "Priority only used in Priority Scheduling mode");
            });

        // Set initial state for FCFS
        preemptiveCheck->setEnabled(false);
        preemptiveCheck->setChecked(false);
        preemptiveCheck->setToolTip("FCFS is always non-preemptive");
        
        // Simulation Mode
        modeGroup = new QGroupBox("Simulation Mode");
        QVBoxLayout* modeLayout = new QVBoxLayout();
        liveRadio = new QRadioButton("Live Visualization");
        liveRadio->setChecked(true);
        stepRadio = new QRadioButton("Immediate Visualization");
        modeLayout->addWidget(liveRadio);
        modeLayout->addWidget(stepRadio);
        modeGroup->setLayout(modeLayout);
        mainLayout->addWidget(modeGroup);

        // Process Configuration
        QGroupBox* processGroup = new QGroupBox("Process Configuration");
        QVBoxLayout* processLayout = new QVBoxLayout();

        // Process Input Table
        QGroupBox* inputGroup = new QGroupBox("Add Processes");
        QGridLayout* inputLayout = new QGridLayout();

        inputLayout->addWidget(new QLabel("Process Name:"), 0, 0);
        nameEdit = new QLineEdit();
        inputLayout->addWidget(nameEdit, 0, 1);

        inputLayout->addWidget(new QLabel("Priority:"), 1, 0);
        prioritySpin = new QSpinBox();
        prioritySpin->setRange(0, 100);
        prioritySpin->setValue(0);
        prioritySpin->setEnabled(false);
        inputLayout->addWidget(prioritySpin, 1, 1);

        inputLayout->addWidget(new QLabel("Arrival Time:"), 2, 0);
        arrivalSpin = new QSpinBox();
        arrivalSpin->setRange(0, 100);
        arrivalSpin->setValue(0);
        inputLayout->addWidget(arrivalSpin, 2, 1);

        inputLayout->addWidget(new QLabel("Burst Time:"), 3, 0);
        burstSpin = new QSpinBox();
        burstSpin->setRange(1, 100);
        burstSpin->setValue(1);
        inputLayout->addWidget(burstSpin, 3, 1);

        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &SetupDialog::addProcess);
        inputLayout->addWidget(addButton, 4, 0, 1, 2);

        inputGroup->setLayout(inputLayout);
        processLayout->addWidget(inputGroup);

        // Process Table
        processTable = new QTableWidget();
        processTable->setColumnCount(4);
        processTable->setHorizontalHeaderLabels({ "Name", "Priority", "Arrival", "Burst" });
        processTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        processLayout->addWidget(processTable);

        // Table controls
        QHBoxLayout* tableControls = new QHBoxLayout();
        //QPushButton* removeButton = new QPushButton("Remove Selected");
        //connect(removeButton, &QPushButton::clicked, this, &SetupDialog::removeSelectedProcess);
        //tableControls->addWidget(removeButton);

        //QPushButton* clearButton = new QPushButton("Clear All");
        //connect(clearButton, &QPushButton::clicked, this, [this]() {
        //    processTable->setRowCount(0);
        //    });
        //tableControls->addWidget(clearButton);
        processLayout->addLayout(tableControls);

        processGroup->setLayout(processLayout);
        mainLayout->addWidget(processGroup);

        // Random processes option
        //randomGroup = new QGroupBox("Random Process Generation");
        //QGridLayout* randomLayout = new QGridLayout();
        //randomCheck = new QCheckBox("Add random processes during simulation");
        //randomCheck->setChecked(true);
        //randomLayout->addWidget(randomCheck, 0, 0, 1, 2);

        //randomLayout->addWidget(new QLabel("Interval:"), 1, 0);
        //intervalSpin = new QSpinBox();
        //intervalSpin->setRange(1, 60);
        //intervalSpin->setValue(5);
        //intervalSpin->setSuffix(" sec");
        //randomLayout->addWidget(intervalSpin, 1, 1);

        //randomLayout->addWidget(new QLabel("Max Processes:"), 2, 0);
        //maxRandomSpin = new QSpinBox();
        //maxRandomSpin->setRange(1, 50);
        //maxRandomSpin->setValue(10);
        //randomLayout->addWidget(maxRandomSpin, 2, 1);

        //randomGroup->setLayout(randomLayout);
        //mainLayout->addWidget(randomGroup);

        // Dialog Buttons
        QDialogButtonBox* buttons = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
            Qt::Horizontal, this);
        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        mainLayout->addWidget(buttons);
    }

    bool isLiveMode() const { return liveRadio->isChecked(); }
    QString getAlgorithm() const {
        QString algo = algorithmCombo->currentText();
        if (algo == "Shortest Job First (SJF)" && isPreemptive()) {
            return "Shortest Job First (SJF)";
        }
        return algo;
    }

    bool isPreemptive() const {
        // Round Robin is always preemptive regardless of checkbox
        if (algorithmCombo->currentIndex() == 3) return true;
        return preemptiveCheck->isChecked();
    }
    int getTimeQuantum() const { return quantumSpin->value(); }
    bool getRandomProcesses() const { return randomCheck->isChecked(); }
    int getRandomInterval() const { return intervalSpin->value(); }
    int getMaxRandomProcesses() const { return maxRandomSpin->value(); }

    /*QList<ProcessInfo> getProcesses() const {
        QList<ProcessInfo> processes;
        for (int i = 0; i < processTable->rowCount(); ++i) {
            ProcessInfo info;
            info.name = processTable->item(i, 0)->text();
            info.priority = processTable->item(i, 1)->text().toInt();
            info.arrival = processTable->item(i, 2)->text().toInt();
            info.burst = processTable->item(i, 3)->text().toInt();
            info.preemptive = (processTable->item(i, 4)->text() == "Yes");
            processes.append(info);
        }
        return processes;
    }*/

private slots:
    void addProcess() {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Process name cannot be empty!");
            return;
        }

        int row = processTable->rowCount();
        processTable->insertRow(row);

        processTable->setItem(row, 0, new QTableWidgetItem(name));
        processTable->setItem(row, 1, new QTableWidgetItem(QString::number(prioritySpin->value())));
        processTable->setItem(row, 2, new QTableWidgetItem(QString::number(arrivalSpin->value())));
        processTable->setItem(row, 3, new QTableWidgetItem(QString::number(burstSpin->value())));
        process* pro = new process(name.toStdString(), arrivalSpin->value(), burstSpin->value(), prioritySpin->value());
        sched->addProcess(pro);
        // Clear input fields
        nameEdit->clear();
        prioritySpin->setValue(0);
        arrivalSpin->setValue(0);
        burstSpin->setValue(1);
    }

    void removeSelectedProcess() {
        QModelIndexList selected = processTable->selectionModel()->selectedRows();
        QList<int> rows;
        for (const QModelIndex& index : selected) {
            rows.append(index.row());
        }

        // Remove in reverse order to avoid shifting issues
        std::sort(rows.begin(), rows.end(), std::greater<int>());
        for (int row : rows) {
            processTable->removeRow(row);
        }
    }
    

private:
    /*struct ProcessInfo {
        QString name;
        int priority;
        int arrival;
        int burst;
        bool preemptive;
    };*/
    scheduler* sched = scheduler::getInstance();

    QComboBox* algorithmCombo;
    QCheckBox* preemptiveCheck;
    QGroupBox* quantumGroup;
    QSpinBox* quantumSpin;
    QGroupBox* modeGroup;
    QRadioButton* liveRadio;
    QRadioButton* stepRadio;
    QGroupBox* randomGroup;
    QCheckBox* randomCheck;
    QSpinBox* intervalSpin;
    QSpinBox* maxRandomSpin;

    // Process input fields
    QLineEdit* nameEdit;
    QSpinBox* prioritySpin;
    QSpinBox* arrivalSpin;
    QSpinBox* burstSpin;
    QTableWidget* processTable;
};
#include "main.moc"



int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Get scheduler instance and add initial processes
    scheduler* sched = scheduler::getInstance();
    /*sched->addProcess(new process("p1", 0, 10, 3));
    sched->addProcess(new process("p2", 0, 1, 1));
    sched->addProcess(new process("p3", 2, 2, 3));
    sched->addProcess(new process("p4", 3, 1, 4));
    sched->addProcess(new process("p5", 4, 5, 2));*/

    // Show setup dialog
    SetupDialog setupDialog;
    if (setupDialog.exec() != QDialog::Accepted) {
        return 0; // Exit if setup is cancelled
    }

    // Apply selected options
    sched->live = setupDialog.isLiveMode();
    QString algorithmName = setupDialog.getAlgorithm();
    algorithm* algo = nullptr;

    if (algorithmName == "First-Come First-Served (FCFS)") {
        algo = new FCFS();
    }
    else if (algorithmName == "Shortest Job First (SJF)") {
        algo = new SJF();
    }
    else if (algorithmName == "Priority Scheduling") {
        algo = new Priority();
    }
    else if (algorithmName == "Round Robin") {
        algo = new RR(setupDialog.getTimeQuantum());
    }

    if (algo) {
        algo->preemptive = setupDialog.isPreemptive();
    }

    // Start the main window
    MainWindow mainWindow(sched->processes, sched->processMutex, sched, algo);
    livePreview = sched->live;
    if (!livePreview) finished = true;
    mainWindow.show();

    return app.exec();
}
