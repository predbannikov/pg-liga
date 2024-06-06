#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

#include "settingsmanager.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    

    ui->setupUi(this);

    QStringList addresses = SettingsManager::instance()->listAddressDevices();
    if (addresses.isEmpty()){
//        ui->tabInstrument->setEnabled(false);
    } else {
        ui->cmbInstrument->addItems(addresses);
    }



    /* Network */
//    ui->spinMaxAddress->setValue(SettingsManager::instance()->maxAddress());
//    ui->spinPollPeriod->setValue(SettingsManager::instance()->pollPeriod());
//    ui->spinTimeout->setValue(SettingsManager::instance()->timeout());
//    ui->spinTimeoutCount->setValue(SettingsManager::instance()->timeoutCount());
//    ui->chkEnableAvoidIdenticalRequest->setChecked(SettingsManager::instance()->avoidIdenticalRequest());

//    ui->chkEnableAutoStart->setChecked(SettingsManager::instance()->autoStart());
//    ui->chkEnableIdleRestore->setChecked(SettingsManager::instance()->idleRestore());
//    ui->spinIdleRestoreTime->setValue(SettingsManager::instance()->idleRestoreTime());
    /* Paths */
//    ui->editDataSavePath->setText(SettingsManager::instance()->dataPath());
//    ui->editDatabasePath->setText(SettingsManager::instance()->databasePath());
//    ui->editArchivePath->setText(SettingsManager::instance()->archivePath());
//    ui->editReportPath->setText(SettingsManager::instance()->reportPath());
//    ui->editTemplatePath->setText(SettingsManager::instance()->templatePath());
//    ui->editRestorePath->setText(SettingsManager::instance()->restorePath());
    /* Misc */
//    ui->chkEnableTips->setChecked(SettingsManager::instance()->tipsEnabled());
    /* Experiment */
//    ui->spinMaxCriterionCount->setValue(SettingsManager::instance()->maxCritetionCount());
//    ui->chkSpecimenChange->setChecked(SettingsManager::instance()->specimenChangeEnabled());
//    ui->chkEnableRelaxation->setChecked(SettingsManager::instance()->relaxationEnabled());
//    ui->chkEnableSensorReadAll->setChecked(SettingsManager::instance()->readAllSensorsEnabled());

//    if(ui->chkEnableRelaxation->isChecked()) ui->chkEnableRelaxation->setEnabled(false);
    /* Instrument */
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::onButtonBoxClicked);
    connect(ui->chkLoadFrameStepDistance, &QCheckBox::stateChanged, ui->spinLoadFrameStepDistance, &QDoubleSpinBox::setEnabled);
    connect(ui->chkLoadFrameStepDistance, &QCheckBox::stateChanged, this, &SettingsDialog::onChkUnitStepDistanceStateChanged);
//    connect(ui->chkShearDeviceStepDistance, &QCheckBox::stateChanged, ui->spinShearDeviceStepDistance, &QDoubleSpinBox::setEnabled);
//    connect(ui->chkShearDeviceStepDistance, &QCheckBox::stateChanged, this, &SettingsDialog::onChkUnitStepDistanceStateChanged);

    ui->spinLoadFrameStepDistance->setEnabled(false);
//    ui->spinShearDeviceStepDistance->setEnabled(false);

//    ui->label_8->setVisible(false);
//    ui->label_9->setVisible(false);
//    ui->spinLoadFrameScaling->setVisible(false);
//    ui->spinShearDeviceScaling->setVisible(false);

//    if(ui->cmbInstrumentType->currentIndex() >= Instrument::LIGA_KL1_1Vol_25T) { ui->grpVolumeter->setVisible(true); }
//    else { ui->grpVolumeter->setVisible(false); }

    /* Other */
//    ui->chkDumpingEnable->setChecked(SettingsManager::instance()->getEnableDumpingProtocol());


    setWindowTitle(tr("Настройки системы"));
    ui->tabSettings->setCurrentIndex(0); 
}

SettingsDialog::~SettingsDialog()
{
    emit dialogClosed();
    delete ui;
}

void SettingsDialog::onButtonBoxClicked(QAbstractButton *button)
{
    const auto role = ui->buttonBox->buttonRole(button);

    if(role == QDialogButtonBox::AcceptRole) {
        saveSettings();
        close();

    } else if(role == QDialogButtonBox::ApplyRole) {
        saveSettings();

    } else {
        close();
    }  
}

void SettingsDialog::saveSettings()
{
//    SettingsManager::instance()->setMaxAddress(ui->spinMaxAddress->value());
//    SettingsManager::instance()->setPollPeriod(ui->spinPollPeriod->value());
//    SettingsManager::instance()->setTimeout(ui->spinTimeout->value());
//    SettingsManager::instance()->setTimeoutCount(ui->spinTimeoutCount->value());
//    SettingsManager::instance()->enableAvoidIdenticalRequest(ui->chkEnableAvoidIdenticalRequest->isChecked());
//    SettingsManager::instance()->enableAutoStart(ui->chkEnableAutoStart->isChecked());
//    SettingsManager::instance()->enableIdleRestore(ui->chkEnableIdleRestore->isChecked());
//    SettingsManager::instance()->setIdleRestoreTime(ui->spinIdleRestoreTime->value());

    SettingsManager::instance()->setDataPath(ui->editDataSavePath->text());
    SettingsManager::instance()->setDatabasePath(ui->editDatabasePath->text());
    SettingsManager::instance()->setArchivePath(ui->editArchivePath->text());
    SettingsManager::instance()->setReportPath(ui->editReportPath->text());
    SettingsManager::instance()->setTemplatePath(ui->editTemplatePath->text());
    SettingsManager::instance()->setRestorePath(ui->editRestorePath->text());

    if(ui->cmbInstrument->isEnabled()) {
        InstrumentParameters *parameters = new InstrumentParameters;
        QString instrName = ui->cmbInstrument->currentText();
        parameters->instrumentName = instrName;


//        parameters.instrumentType = ui->cmbInstrumentType->currentIndex();

//        parameters.triaxialChamber.type = ui->cmbChamberType->currentIndex();
//        parameters.triaxialChamber.pistonDiameter = Measurements::Length::fromMillimetres(ui->spinChamberPistonDiameter->value());
//        parameters.triaxialChamber.pistonWeight = Measurements::Force::fromNewtons(ui->spinPistonWeight->value());

//        parameters.loadFrame.friction = Measurements::Force::fromNewtons(ui->spinLoadFrameFriction->value());
//        parameters.loadFrame.scaling = ui->spinLoadFrameScaling->value();
//        parameters.loadFrame.adjustSpeed = Measurements::Speed::fromMillimetresPerMinute(ui->spinLoadFrameSpeed->value());
//        parameters.loadFrame.adjustCoefficient = ui->spinLoadFrameCorrection->value();
//        parameters.loadFrame.stepDistance = Measurements::Length::fromMicrometres(ui->spinLoadFrameStepDistance->value());

//        parameters.shearDevice.friction = Measurements::Force::fromNewtons(ui->spinShearDeviceFriction->value());
//        parameters.shearDevice.scaling = ui->spinShearDeviceScaling->value();
//        parameters.shearDevice.stepDistance = Measurements::Length::fromMicrometres(ui->spinShearDeviceStepDistance->value());

//        parameters.cellVolumeter.multipicationCoefficient = ui->spinVolumeterMultiplication->value();
//        parameters.poreVolumeter.multipicationCoefficient = ui->spinVolumeterMultiplication->value();

        SettingsManager::instance()->setInstrumentParameters(ui->cmbInstrument->currentText(), parameters);
    }

//    SettingsManager::instance()->enableSpecimenChange(ui->chkSpecimenChange->isChecked());
//    SettingsManager::instance()->enableReadAllSensors(ui->chkEnableSensorReadAll->isChecked());
//    SettingsManager::instance()->setMaxCritetionCount(ui->spinMaxCriterionCount->value());

//    SettingsManager::instance()->enableTips(ui->chkEnableTips->isChecked());
//    SettingsManager::instance()->setEnableDumpingProtocol(ui->chkDumpingEnable->isChecked());



    SettingsManager::instance()->save();
    SettingsManager::instance()->load();

    
}

void SettingsDialog::on_btnSetDataSavePath_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, QString(), ui->editDataSavePath->text());

    if (path.isEmpty()) {
        
        return;
    } else {
        ui->editDataSavePath->setText(QDir::toNativeSeparators(path));
    }  
}

void SettingsDialog::on_btnSetDatabasePath_clicked()
{
    const auto path = QFileDialog::getOpenFileName(this, QString(), ui->editDatabasePath->text());

    if(path.isEmpty()) {
        
        return;
    } else {
        ui->editDatabasePath->setText(QDir::toNativeSeparators(path));
    }
}

void SettingsDialog::on_btnSetArchivePath_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, QString(), ui->editArchivePath->text());

    if (path.isEmpty()) {
        
        return;
    } else {
        ui->editArchivePath->setText(QDir::toNativeSeparators(path));
    }
}

void SettingsDialog::on_btnSetReportPath_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, QString(), ui->editReportPath->text());

    if (path.isEmpty()) {
        
        return;
    } else {
        ui->editReportPath->setText(QDir::toNativeSeparators(path));
    }
}

void SettingsDialog::on_btnSetTemplatePath_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, QString(), ui->editTemplatePath->text());

    if (path.isEmpty()) {
        
        return;
    } else {
        ui->editTemplatePath->setText(QDir::toNativeSeparators(path));
    }
}

void SettingsDialog::on_btnSetRestorePath_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, QString(), ui->editRestorePath->text());

    if (path.isEmpty()) {
        
        return;
    } else {
        ui->editRestorePath->setText(QDir::toNativeSeparators(path));
    } 
}

void SettingsDialog::on_cmbInstrument_currentIndexChanged(const QString &text)
{

//    ui->cmbInstrumentType->setCurrentIndex(parameters.instrumentType);
//    ui->cmbChamberType->setCurrentIndex(parameters.triaxialChamber.type);
//    ui->spinChamberPistonDiameter->setValue(parameters.triaxialChamber.pistonDiameter.millimetres());
//    ui->spinPistonWeight->setValue(parameters.triaxialChamber.pistonWeight.newtons());
//    ui->spinLoadFrameFriction->setValue(int(parameters.loadFrame.friction.newtons()));
//    ui->spinShearDeviceFriction->setValue(int(parameters.shearDevice.friction.newtons()));
//    ui->spinLoadFrameScaling->setValue(parameters.loadFrame.scaling);
//    ui->spinShearDeviceScaling->setValue(parameters.shearDevice.scaling);
//    ui->spinLoadFrameSpeed->setValue(parameters.loadFrame.adjustSpeed.millimetresPerMinute());
//    ui->spinLoadFrameCorrection->setValue(parameters.loadFrame.adjustCoefficient);
//    ui->spinLoadFrameStepDistance->setValue(parameters.loadFrame.stepDistance.micrometres());
//    ui->spinShearDeviceStepDistance->setValue(parameters.shearDevice.stepDistance.micrometres());
//    ui->spinVolumeterMultiplication->setValue(parameters.cellVolumeter.multipicationCoefficient);
}

void SettingsDialog::on_cmbChamberType_currentIndexChanged(int index)
{
//    ui->spinChamberPistonDiameter->setEnabled(index == InstrumentParameters::ChamberTypeA);
//    ui->spinPistonWeight->setEnabled(index == InstrumentParameters::Oedometer);
}

void SettingsDialog::on_cmbInstrumentType_currentIndexChanged(int index)
{
    const auto loadFrameStepDistance = ui->spinLoadFrameStepDistance->value();

//    if (index >= Instrument::LIGA_KL0_5T && index <= Instrument::LIGA_KL1_2Vol_5T) {
//        if (loadFrameStepDistance == 0.3125 || loadFrameStepDistance == 0.0025) {
//            ui->spinLoadFrameStepDistance->setValue(0.05);
//        }
////        ui->grpVolumeter->setVisible(false);
//    }
//    else if (index >= Instrument::LIGA_KL1_1Vol_25T) {
//        if (loadFrameStepDistance == 0.3125 || loadFrameStepDistance == 0.05) {
//            ui->spinLoadFrameStepDistance->setValue(0.0025);
//        }
////        ui->grpVolumeter->setVisible(true);
//    }
//    else if (index <= Instrument::LIGA_KL1S_2Vol_1T) {
//        if (loadFrameStepDistance == 0.05 || loadFrameStepDistance == 0.0025) {
//            ui->spinLoadFrameStepDistance->setValue(0.3125);
//        }
////        ui->grpVolumeter->setVisible(false);
//    }
}

void SettingsDialog::onChkUnitStepDistanceStateChanged(int state)
{
    if (state > 0) {
        QMessageBox::information(this, "Внимание!", "Изменение данной настройки может привести к неправильному "
                                                    "расчёту скоростей и перемещений органа прибора. "
                                                    "Все изменения производятся пользователем на свой страх и риск.");
    } else {}
}

void SettingsDialog::on_btnAddDeviceAddress_clicked()
{
    QString address = ui->leAddressDevice->text();
    auto map = SettingsManager::instance()->getServers();
    map.insert(address, address);
    SettingsManager::instance()->setServers(map);
    ui->cmbInstrument->clear();
    ui->cmbInstrument->addItems(map.keys());
}


void SettingsDialog::on_cmbInstrument_currentTextChanged(const QString &arg1)
{
    const auto parameters = SettingsManager::instance()->instrumentParameters(arg1);

}


void SettingsDialog::on_btnRmoveDevice_clicked()
{
    ui->chkClearSettings->setChecked(false);
    SettingsManager::instance()->removeDevice(ui->cmbInstrument->currentText());
}

