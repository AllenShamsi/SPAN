#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#include <QDialog>
#include "spanFile.h"

namespace Ui {
class FileInfoDialog;
}

class FileInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileInfoDialog(QWidget *parent = nullptr);
    ~FileInfoDialog();

    // Call this to fill the dialog based on the loaded span file
    void setSpanFile(const spanFile &file);

private:
    Ui::FileInfoDialog *ui;
};

#endif // FILEINFODIALOG_H
