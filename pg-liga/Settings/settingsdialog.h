#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class QAbstractButton;
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void onButtonBoxClicked(QAbstractButton *button);

    void on_btnSetDataSavePath_clicked();
    void on_btnSetDatabasePath_clicked();
    void on_btnSetArchivePath_clicked();
    void on_btnSetReportPath_clicked();
    void on_btnSetTemplatePath_clicked();
    void on_btnSetRestorePath_clicked();

    void on_cmbInstrument_currentIndexChanged(const QString &text);
    void on_cmbChamberType_currentIndexChanged(int index);
    void on_cmbInstrumentType_currentIndexChanged(int index);
    void onChkUnitStepDistanceStateChanged(int state);

signals:
    void dialogClosed();

private:
    void saveSettings();
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
