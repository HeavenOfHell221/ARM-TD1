#include "dicom_viewer.h"

#include <iostream>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

using namespace std;

DicomViewer::DicomViewer(QWidget *parent) : QMainWindow(parent) {
  // Setting menu
  QMenu *file_menu = menuBar()->addMenu("&File");
  QAction *open_action = file_menu->addAction("&Open");
  open_action->setShortcut(QKeySequence::Open);
  QObject::connect(open_action, SIGNAL(triggered()), this, SLOT(openDicom()));
  QAction *help_action = file_menu->addAction("&Help");
  help_action->setShortcut(QKeySequence::HelpContents);
  QObject::connect(help_action, SIGNAL(triggered()), this, SLOT(showStats()));
}

DicomViewer::~DicomViewer() {}

void DicomViewer::openDicom() {

  ostringstream msg_oss;
  msg_oss << "Your file have been succesfully selected" << endl;

  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/path/to/file/",tr("Dcm File Format (*.dcm)"));
  string str = fileName.toStdString();
  const char* p = str.c_str();
  
  DcmFileFormat fileformat;
  OFCondition status = fileformat.loadFile(p);

  if (status.good()){

    // (1/11) The dicom deftag DCM_PatientName seems to be the best
    if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good()) {
      cout << "Patient's Name: " << patientName << endl;
    } else {
      cerr << "Error: cannot access Patient's Name." << endl;
    }

    /*-----------------------------*/

    // (2/11) The dicom deftag DCM_InstanceNumber seems to be the best
    if (fileformat.getDataset()->findAndGetOFString(DCM_InstanceNumber, instanceNumber).good()) {
      cout << "Instance number: " << instanceNumber << endl;
    } else {
      cerr << "Error: cannot access Instance number!" << endl;
    }

    /*-----------------------------*/

    // (3/11) The dicom deftag DCM_AcquisitionNumber seems to be the best
    if (fileformat.getDataset()->findAndGetOFString(DCM_AcquisitionNumber, acquisitionNumber).good()) {
      cout << "Acquisition number: " << acquisitionNumber << endl;
    } else {
      cerr << "Error: cannot access Acquisition number !" << endl;
    }

    /*-----------------------------*/

    // (4/11) The dicom deftag DCM_TransferSyntaxUID don't have the same label as required but seems to be 
    //        the best among the others
    if (fileformat.getDataset()->findAndGetOFString(DCM_TransferSyntaxUID, transferSyntaxUID).good()) {
      cout << "Transfer Syntax UID: " << transferSyntaxUID << endl;
    } else {
      cerr << "Error: cannot access Transfer Syntax UID!" << endl;
    }

    // (5/11) The dicom deftag DCM_ReferencedFrameNumbers don't have the same label as required but seems to be 
    //        the best among the others
    if (fileformat.getDataset()->findAndGetOFString(DCM_ReferencedFrameNumbers, referencedFrameNumbers).good()) {
      cout << "Referenced frame numbers: " << referencedFrameNumbers << endl;
    } else {
      cerr << "Error: cannot access Referenced frame numbers!" << endl;
    }
    
    // (6/11) The dicom deftag DCM_PatientSize don't have the same label as required but seems to be 
    //        the best among the others
    if (fileformat.getDataset()->findAndGetOFString(DCM_PatientSize, patientSize).good()) {
      cout << "patientSize: " << patientSize << endl;
    } else {
      cerr << "Error: cannot access patientSize!" << endl;
    }

  } else { 
    cerr << "Error: cannot read DICOM file (" << status.text() << ")" << endl;
  }

  QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}

void DicomViewer::showStats() {
  ostringstream msg_oss;
  msg_oss << "Patient: " << patientName << endl;
  msg_oss << "Instance number: " << instanceNumber << endl;
  msg_oss << "Acquisition number: "<< acquisitionNumber << endl;
  msg_oss << "Original transfer syntax: " << transferSyntaxUID << endl;
  msg_oss << "Nb frames: " << referencedFrameNumbers << endl;
  msg_oss << "Size: " << patientSize << endl;

  msg_oss << "Allowed values: [TODO,TODO]" << endl;
  msg_oss << "Used values: [TODO,TODO]" << endl;
  msg_oss << "Window: [TODO,TODO]" << endl;
  msg_oss << "Slope: TODO" << endl;
  msg_oss << "Intercept: TODO" << endl;

  QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}





