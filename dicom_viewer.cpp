#include "dicom_viewer.h"
#include "dcmtk/dcmimage/diregist.h"

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
  QString fileName_Qt = QFileDialog::getOpenFileName(this, tr("Open File"),"..",tr("Dcm File Format (*.dcm)"));
  string fileName_str = fileName_Qt.toStdString();
  const char* fileName = fileName_str.c_str();
  
  ostringstream msg_oss;
  msg_oss << "Your file have been succesfully selected" << endl;
  
  DcmFileFormat fileformat;
  OFCondition status = fileformat.loadFile(fileName);

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

    /*-----------------------------*/

    // (5/11) The dicom deftag DCM_ReferencedFrameNumbers don't have the same label as required but seems to be 
    //        the best among the others
    if (fileformat.getDataset()->findAndGetOFString(DCM_ReferencedFrameNumbers, referencedFrameNumbers).good()) {
      cout << "Referenced frame numbers: " << referencedFrameNumbers << endl;
    } else {
      cerr << "Error: cannot access Referenced frame numbers!" << endl;
    }

    /*-----------------------------*/
   
    // (6/11) The dicom deftag DCM_PatientSize don't have the same label as required but seems to be 
    //        the best among the others
    if (fileformat.getDataset()->findAndGetOFString(DCM_PatientSize, patientSize).good()) {
      cout << "patientSize: " << patientSize << endl;
    } else {
      cerr << "Error: cannot access patientSize!" << endl;
    }

    /*-----------------------------*/

    // (7/11) Allowed values : If it's a range
    //  - DCM_NominalMinEnergy / DCM_NominalMaxEnergy
    //  - DCM_MinimumStoredValueMapped / DCM_MaximumStoredValueMapped
    //  - DCM_MinDensity / DCM_MaxDensity
    //  - DCM_DVHMinimumDose / DCM_DVHMaximumDose
    //  - DCM_ChannelMinimumValue / DCM_ChannelMaximumValue

    //  There is too many possibilities to choose randomly one among them, so it's possibly none of them and more likely  
    //  a range between 0 and 2^DCM_BitsAllocated.
    if (fileformat.getDataset()->findAndGetOFString(DCM_BitsAllocated, bitsAllocated).good()) {
      cout << "0,bitsAllocated: " << "0" << "," << bitsAllocated << endl;
    } else {
      cerr << "Error: cannot access bitsAllocated!" << endl;
    }

    /*-----------------------------*/

    // (8/11) Used values :
    //  - [0; 2^DCM_BitsStored]
    //  - [0; 2^DCM_HighBit]
    if (fileformat.getDataset()->findAndGetOFString(DCM_BitsStored, bitsStored).good()
    &&  fileformat.getDataset()->findAndGetOFString(DCM_HighBit, highBit).good()
    ) {
      cout << "bitsStored,highBit: " << bitsStored << "," << highBit << endl;
    } else {
      cerr << "Error: cannot access bitsStored or highBit !" << endl;
    }

    /*-----------------------------*/

    // (9/11) Window :
    //  - [DCM_EnergyWindowLowerLimit; DCM_EnergyWindowUpperLimit] 
    //  - Very likely : DCM_WindowCenter and DCM_WindowWidth
    if (fileformat.getDataset()->findAndGetOFString(DCM_WindowCenter, windowCenter).good()
    &&  fileformat.getDataset()->findAndGetOFString(DCM_WindowWidth, windowWidth).good()
    ) {
      cout << "windowCenter,windowWidth: " << windowCenter << "," << windowWidth << endl;
    } else {
      cerr << "Error: cannot access windowCenter or windowWidth !" << endl;
    }

    /*-----------------------------*/

    // (10/11) Slop : Maybe DCM_RescaleSlope
    if (fileformat.getDataset()->findAndGetOFString(DCM_RescaleSlope, rescaleSlope).good()) {
      cout << "rescaleSlope: " << rescaleSlope << endl;
    } else {
      cerr << "Error: cannot access rescaleSlope!" << endl;
    }

    /*-----------------------------*/

    //(11/11) Intercept : Maybe DCM_RescaleIntercept
    if (fileformat.getDataset()->findAndGetOFString(DCM_RescaleIntercept, rescaleIntercept).good()) {
      cout << "rescaleIntercept: " << rescaleIntercept << endl;
    } else {
      cerr << "Error: cannot access rescaleIntercept!" << endl;
    }

  } else { 
    cerr << "Error: cannot read DICOM file (" << status.text() << ")" << endl;
  }

  /* ------------------------------ Part 3 : ------------------------------ */

  DicomImage *image = new DicomImage(fileName);
  if (image != NULL)
  {
    EI_Status status = image->getStatus();

    switch (status)
    {
      case EIS_Normal:
      {
        Uint8 *pixelData = (Uint8 *)(image->getOutputData(16 /* bits per sample */));
        if (pixelData != NULL)
        {
          /* do something useful with the pixel data */
        }
        break;
      }
      default:
      {
        cerr << "Error: cannot load DICOM image (" << DicomImage::getString(image->getStatus()) << ")" << endl;

        /*
          gabriel@Heaven:~/GitHub/ARM-TD1/build$ dcm2pnm ../Unknown\ Study/CT\ PLAIN\ THIN/CT000001.dcm test.png
          E: can't change to unencapsulated representation for pixel data
          E: can't determine 'PhotometricInterpretation' of decompressed image
          E: mandatory attribute 'PhotometricInterpretation' is missing or can't be determined
          F: Missing attribute

          Same error as the code with the command line for convert DICOM images to .png.
          The error is with all extension (ppm, png, ..), so it's either the DICOM file or it's not the right conversion.
            -> Add missing attributes? or fill them
            -> Intermediate conversion?
        */
        break;
      }
    }
  }
  delete image;
  
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
  msg_oss << "Allowed values: " << "[" << "0" << "," << bitsAllocated << "]"<< endl;
  msg_oss << "Used values: " << "[" << bitsStored << "," << highBit << "]" << endl;
  msg_oss << "Window: " << "[" << windowCenter << "," << windowWidth << "]" << endl;
  msg_oss << "Slope: " << rescaleSlope << endl;
  msg_oss << "Intercept: " << rescaleIntercept << endl;

  QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}





