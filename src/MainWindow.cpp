#include "ui_MainWindow.h"
#include "MainWindow.h"
#include "AudioItemWidget.h"
#include "TagItemWidget.h"
#include "TagAssignmentDialog.h"
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QIcon>
#include <sstream>
#include <algorithm>
#include <cctype>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/assets/images/icons/luna-lone-icon.png"));
    m_audio_player = new AudioPlayer(this);
    m_text_animator = new TextAnimator(ui->avatarText, this);

    ui->playbackProgress->setStyleSheet("QProgressBar::chunk {background: rgba(191, 148, 255, 1);}");

    connect(ui->addAudioButton, &QPushButton::clicked, this, &MainWindow::onAddAudioClicked);
    connect(ui->selectFileButton, &QPushButton::clicked, this, &MainWindow::onSelectFileClicked);
    connect(ui->addTagButton, &QPushButton::clicked, this, &MainWindow::onAddTagClicked);

    connect(ui->mainViewAction, &QAction::triggered, this, &MainWindow::showMainView);
    connect(ui->audioListAction, &QAction::triggered, this, &MainWindow::showAudioListView);
    connect(ui->tagListAction, &QAction::triggered, this, &MainWindow::showTagListView);

    connect(ui->mainPageButton, &QPushButton::clicked, this, &MainWindow::showMainView);
    connect(ui->audioPageButton, &QPushButton::clicked, this, &MainWindow::showAudioListView);
    connect(ui->tagPageButton, &QPushButton::clicked, this, &MainWindow::showTagListView);
    
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlayClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
    connect(m_audio_player, &AudioPlayer::positionChanged, this, &MainWindow::onAudioPositionChanged);
    connect(m_audio_player, &AudioPlayer::durationChanged, this, &MainWindow::onAudioDurationChanged);

    connect(ui->promptEdit, &QLineEdit::returnPressed, this, &MainWindow::onPromptEntered);

    QString dataDir = QDir::currentPath() + "/data";
    QDir().mkpath(dataDir);
    QString dbPath = dataDir + "/db/database.db";

    database = std::make_shared<Database>(dbPath.toStdString());
    if (!database->init()) {
        QMessageBox::critical(this, "Database Error", "Failed to initialize the database.");
    }

    this->showMainView();
    this->refreshAudioList();
    this->refreshTagList();

    m_text_animator->startTyping("Hello! I'm Luna. What can I play for you?");
}

MainWindow::~MainWindow() 
{
    delete ui, m_audio_player;
};

void MainWindow::onSelectFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Select Audio File", QDir::homePath(), "Audio File (*.mp3 *.wav *.ogg)"
    );

    if (!filePath.isEmpty()) {
        selectedFilePath = filePath;
        ui->statusBar->showMessage("Selected file: " + QFileInfo(filePath).fileName(), 3000);

        ui->selectedFileLabel->setText(QFileInfo(filePath).fileName());
    }
}

void MainWindow::onAddAudioClicked() 
{
    if (selectedFilePath.isEmpty()) {
        QMessageBox::warning(this, "No File Selected", "Please select an audio file first.");
        return;
    }

    QString displayName = ui->audioNameEdit->text().trimmed();
    if (displayName.isEmpty()) {
        QMessageBox::warning(this, "No Name", "Please enter a name for the audio file.");
        return;
    }

    QString dataDir = QDir::currentPath() + "/data/audio";
    QDir().mkpath(dataDir);
    
    QString destPath = dataDir + "/" + displayName + "." + QFileInfo(selectedFilePath).suffix();

    if (QFile::exists(destPath)) {
        QMessageBox::warning(this, "File Exists", "An audio file with this name already exists.");
        return;
    }

    if (database) {
        if (QFile::copy(selectedFilePath, destPath) and 
            database->addAudioEntry(displayName.toStdString(), destPath.toStdString())) {
            
            this->refreshAudioList();

            ui->statusBar->showMessage("Uploaded: " + displayName, 3000);
        } else {
            QMessageBox::warning(this, "Upload Failed", "Failed to upload the audio file.");
            return;
        }
    }

    selectedFilePath.clear();
    ui->selectedFileLabel->setText("...");
    ui->audioNameEdit->clear();
}

void MainWindow::onAddTagClicked() 
{
    QString tagName = ui->tagNameEdit->text().trimmed();

    if (tagName.isEmpty()) {
        QMessageBox::warning(this, "No Tag Name", "Please enter a tag name.");
        return;
    }

    if (database) {
        if (database->addTagEntry(tagName.toStdString())) {

            this->refreshTagList();

            ui->statusBar->showMessage("Added tag: " + tagName, 3000);
        } else {
            QMessageBox::warning(this, "Database Error", "Failed to add tag to the database.");
            return;
        }
    }

    ui->tagNameEdit->clear();
}

void MainWindow::showMainView() 
{
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
}

void MainWindow::showAudioListView() 
{
    ui->stackedWidget->setCurrentWidget(ui->pageAudioList);
}

void MainWindow::showTagListView() 
{
    ui->stackedWidget->setCurrentWidget(ui->pageTagList);
}

void MainWindow::onAssignTags(int audioId, const QString &audioName)
{
    if (!database) return;

    TagAssignmentDialog dlg(audioId, audioName, database, this);
    dlg.exec();

    this->refreshAudioList();
}

void MainWindow::onDeleteAudio(int audioId)
{
    if (database) {
        database->deleteAudioEntry(audioId);
    }

    this->refreshAudioList();
}

void MainWindow::onDeleteTag(int tagId)
{
    if (database) {
        database->deleteTagEntry(tagId);
    }

    this->refreshTagList();
    this->refreshAudioList();
}

void MainWindow::refreshAudioList()
{
    ui->audioListWidget->clear();

    if (database) {
        auto audioEntries = database->getAllAudioEntries();
        for (const auto &entry : audioEntries) {
            QListWidgetItem *item = new QListWidgetItem(ui->audioListWidget);
            std::vector<TagEntry> tags = database->getTagsForAudio(entry.id);

            AudioItemWidget *widget = new AudioItemWidget(
                entry.id,
                QString::fromStdString(entry.displayName),
                tags,
                this
            );

            connect(widget, &AudioItemWidget::assignTagsClicked, this, &MainWindow::onAssignTags);
            connect(widget, &AudioItemWidget::deleteAudioClicked, this, &MainWindow::onDeleteAudio);

            item->setSizeHint(widget->sizeHint());
            ui->audioListWidget->addItem(item);
            ui->audioListWidget->setItemWidget(item, widget);
        }
    }
}

void MainWindow::refreshTagList()
{
    ui->tagListWidget->clear();

    if (database) {
        auto tagEntries = database->getAllTagEntries();
        for (const auto &entry : tagEntries) {
            QListWidgetItem *item = new QListWidgetItem(ui->tagListWidget);
            TagItemWidget *widget = new TagItemWidget(
                entry.id,
                QString::fromStdString(entry.tagName),
                this
            );

            connect(widget, &TagItemWidget::deleteTagClicked, this, &MainWindow::onDeleteTag);

            item->setSizeHint(widget->sizeHint());
            ui->tagListWidget->addItem(item);
            ui->tagListWidget->setItemWidget(item, widget);
        }
    }
}

void MainWindow::onPlayClicked()
{
    m_audio_player->play();
}

void MainWindow::onPauseClicked()
{
    m_audio_player->pause();
}

void MainWindow::onStopClicked()
{
    m_audio_player->stop();
}

void MainWindow::onAudioPositionChanged(qint64 position)
{
    ui->playbackProgress->setValue(position);
}

void MainWindow::onAudioDurationChanged(qint64 duration)
{
    if(duration <= 0)
        ui->playbackProgress->setValue(0);
    else
        ui->playbackProgress->setMaximum(duration);
}

void MainWindow::onPromptEntered()
{
    QString prompt = ui->promptEdit->text().trimmed();

    if (prompt.isEmpty()) return;

    processPrompt(prompt.toStdString());
    ui->promptEdit->clear();
}

static std::vector<std::string> splitWords(const std::string &text)
{
    std::vector<std::string> result;
    std::stringstream ss(text);
    std::string word;

    while (ss >> word)
    {
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c){return std::tolower(c);});
        result.push_back(word);
    }

    return result;
}

void MainWindow::processPrompt(const std::string &prompt)
{
   std::vector<std::string> words = splitWords(prompt);

   std::vector<TagEntry> allTags = database->getAllTagEntries();

   std::vector<int> matchedTagIds;

   for (const TagEntry &tag : allTags)
   {
    std::string lowerTag = tag.tagName;
    std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), [](unsigned char c){return std::tolower(c);});

    if (std::find(words.begin(), words.end(), lowerTag) != words.begin())
    {
        matchedTagIds.push_back(tag.id);
    }

   }

   if (matchedTagIds.empty()) {
    m_text_animator->startTyping("I'm sorry. I do not recognize any tags in this request.");
    return;
   }
   

   std::pair<bool, AudioEntry> best_choice = database->chooseBestWeightedMatch(matchedTagIds);

   if (!best_choice.first) {
    m_text_animator->startTyping("I'm sorry. I could not find any audio matching this request.");
    return;
   } 

   m_text_animator->startTyping("Here you go. Enjoy!");

   QString audioPath = QString::fromStdString(best_choice.second.filePath);
   m_audio_player->select(audioPath);
   m_audio_player->play();
}
