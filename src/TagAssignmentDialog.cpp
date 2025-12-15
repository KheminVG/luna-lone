#include "ui_TagAssignmentDialog.h"
#include "TagAssignmentDialog.h"
#include <QDialogButtonBox>

TagAssignmentDialog::TagAssignmentDialog(int id, const QString &name, std::shared_ptr<Database> db, QWidget* parent)
    : QDialog(parent), audioId(id), audioName(name), database(db), ui(new Ui::TagAssignmentDialog)
{
    ui->setupUi(this);

    auto allTags = database->getAllTagEntries();
    auto assignedTags = database->getTagsForAudio(audioId);

    for (const auto &tag : assignedTags) {
        selectedTagIds.insert(tag.id);
    }

    for (const auto &tag : allTags) {
        TagChip* tagChip = new TagChip(tag.id, QString::fromStdString(tag.tagName), this);
        tagChip->setSelected(selectedTagIds.count(tag.id) > 0);
        ui->tagContainer->addWidget(tagChip);

        connect(tagChip, &TagChip::toggled, this, &TagAssignmentDialog::onTagToggled);
    }

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TagAssignmentDialog::onSave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

TagAssignmentDialog::~TagAssignmentDialog()
{
    delete ui;
}

void TagAssignmentDialog::onTagToggled(int tagId, bool selected)
{
    if (selected) {
        selectedTagIds.insert(tagId);
    } else {
        selectedTagIds.erase(tagId);
    }
}

void TagAssignmentDialog::onSave()
{
    database->clearAudioTags(audioId);

    for (int tagId : selectedTagIds) {
        database->linkTagToAudio(audioId, tagId);
    }
    
    accept();
}
