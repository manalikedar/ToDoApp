#include "ToDoApps.h"
#include <qfile.h>
#include <qstandardpaths.h>
#include <qmessagebox.h>
#include <qtextstream.h>

ToDoApps::ToDoApps(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    // Set stylesheet
    initStylesheet();

    // Connect the add new task button
    connect(ui.AddNewBtn, SIGNAL(clicked()), this, SLOT(SlotAddNewTask()));
    connect(ui.NewTaskLineEdit, SIGNAL(clicked()), this, SLOT(ui.NewTaskLineEdit->clear()));
    ui.NewTaskLineEdit->setText("Today, I want to...");
    ui.NewDateText->setText(QDate::currentDate().toString());

    QFile file(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "\\toDoFile.txt");
    if (!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream inFile(&file);
    while (!inFile.atEnd())
    {
        QString newTask = inFile.readLine();
        int index = newTask.indexOf(":");
        createNewTask(newTask.mid(index+1), newTask.left(index));
    }
    file.resize(0);
    file.close();
}

ToDoApps::~ToDoApps()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "\\toDoFile.txt");
    if (!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream outFile(&file);
   
    // Get all tasks from the layout and save their details
    QVBoxLayout* vMainLayout = qobject_cast<QVBoxLayout*>(ui.AllNewTasksContents->layout());
    for (int i = 0; i < vMainLayout->count(); ++i) {
        QWidget* widget = vMainLayout->itemAt(i)->widget();
        if (widget) {
            QFrame* hFrame = qobject_cast<QFrame*>(widget);
            if (hFrame) {
                QLabel* taskLabel = hFrame->findChild<QLabel*>("tasklabel");
                QLabel* dateLabel = hFrame->findChild<QLabel*>("datelabel");

                if (taskLabel && dateLabel) {
                    outFile << dateLabel->text() << ":" << taskLabel->text() << "\n";
                }
            }
        }
    }
    file.close();
}

void ToDoApps::initStylesheet() {
    QFile style("style_Generic.css");
    bool ok = style.open(QFile::ReadOnly);
    QString s = QString::fromLatin1(style.readAll());

    setStyleSheet(s);

}

void ToDoApps::SlotAddNewTask() {
    // Get the line edit text
    QString taskName = ui.NewTaskLineEdit->text();
    if (!taskName.isEmpty())
    {
        //Get current date
        QString date = QDate::currentDate().toString();

        createNewTask(taskName, date);
    }

    ui.NewTaskLineEdit->clear();
    ui.NewTaskLineEdit->setFocus();
    // This is how to trigger signal
    //emit SignalAddNewTask(taskName, date);

    // Then in other class/widget connect it like
    //ToDoApps* obj;
    //connect(obj, SIGNAL(SignalAddNewTask(QString, QString)), this, SLOT(SlotToSomethingInThatClass()));
}

void ToDoApps::createNewTask(QString taskName, QString date){

    {
        // The hierarchy of this widget will be like this
        /*
            + Hframe (Frame)
                + Vframe (Frame)
                    + titlelabel (Label)
                    + tasklabel (Label)
                    + datelabel (Label)
                + spacer (Horizontal spacer)
                + deleteBtn (PushButton)
        */
    }
    // Get the parent widget which the widget created to be child in
    QVBoxLayout* vMainLayout = qobject_cast<QVBoxLayout*>(ui.AllNewTasksContents->layout());
   
    // Create Frame for the main widget container
    QFrame* HFrame = new QFrame();  
    HFrame->setFrameStyle(QFrame::StyledPanel);
    // Create Horizontal Box Layout as the Frame layout and also for easily add widget inside it
    // This is like a virtual layout. It doesn't added into ui as a ui things bcos it is just a layout.
    QHBoxLayout* newTask = new QHBoxLayout(HFrame);
    HFrame->setLayout(newTask);

    // Create Frame for the details container; task title, task name, task date created
    QFrame* Vframe = new QFrame();
    QVBoxLayout* taskDetails = new QVBoxLayout(Vframe);
    Vframe->setLayout(taskDetails);

    QLabel* titlelabel = new QLabel(tr("Task #%1").arg(vMainLayout->count())); // task title
  
    titlelabel->setObjectName("titlelabel");
    taskDetails->addWidget(titlelabel);
    QLabel* taskLabel = new QLabel(taskName);
    taskDetails->addWidget(taskLabel);
    QLabel* datelabel = new QLabel(date); // task date created
    taskDetails->addWidget(datelabel);
    titlelabel->setObjectName("titlelabel");
    taskLabel->setObjectName("tasklabel");
    datelabel->setObjectName("datelabel");
    // Insert the task details frame inside main task box layout
    newTask->insertWidget(0, Vframe);

    // Insert horizontal spacer in between Vframe and deleteBtn
    QSpacerItem* spacer = new QSpacerItem(100, 100, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
    newTask->insertSpacerItem(1, spacer);

    // Insert delete button
        QPushButton * deleteBtn = new QPushButton("Delete");
    newTask->insertWidget(2, deleteBtn);

    // Store the current duplicated widget to the delete button property for easy reference
    // This is a useful function if you want to reference any widgets to the specific button
    deleteBtn->setProperty("CurrentTask", QVariant(QVariant::fromValue<QFrame*>(HFrame)));

    // Insert into parent ui frame
    vMainLayout->insertWidget(vMainLayout->count() - 1, HFrame);
    
    // Connect the delete button
    connect(deleteBtn, SIGNAL(clicked()), this, SLOT(SlotDeleteTask()));
}

void ToDoApps::SlotDeleteTask() {
    
    // Get the sender widget
    QPushButton* fromButton = (QPushButton*)sender();

     //Get the widget referenced in the property
    QVariant var;
    var = fromButton->property("CurrentTask");
    QFrame* taskHBox = qvariant_cast<QFrame*>(var);

    taskHBox->deleteLater();
    delete taskHBox;
}
