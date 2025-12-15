#pragma once

#include "TagChip.h"
#include "Database.h"
#include <QDialog>
#include <unordered_set>

namespace Ui {
    class TagAssignmentDialog;
}

class TagAssignmentDialog : public QDialog {
    Q_OBJECT

public:
    explicit TagAssignmentDialog(int id, const QString &name, std::shared_ptr<Database> db, QWidget* parent = nullptr);
    ~TagAssignmentDialog();

private slots:
    void onTagToggled(int tagId, bool selected);
    void onSave();

private:
    Ui::TagAssignmentDialog* ui;
    QString audioName;
    int audioId;
    
    std::shared_ptr<Database> database;
    std::unordered_set<int> selectedTagIds;
};