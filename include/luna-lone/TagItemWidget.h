#pragma once

#include <QWidget>

namespace Ui {
    class TagItemWidget;
}

class TagItemWidget : public QWidget 
{
    Q_OBJECT

public:
    explicit TagItemWidget(int id, const QString &name, QWidget* parent = nullptr);
    ~TagItemWidget();

private:
    Ui::TagItemWidget* ui;
    QString tagName;
    int tagId;

signals:
    void deleteTagClicked(int tagId);

private slots:
    void onDeleteTagClicked();

};
