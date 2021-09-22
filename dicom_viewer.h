#ifndef DICOM_VIEWER_H
#define DICOM_VIEWER_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "dcmtk/dcmdata/dcrledrg.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmjpeg/djdecode.h"

class DicomViewer : public QMainWindow {
  Q_OBJECT

public:
  DicomViewer(QWidget *parent = 0);
  ~DicomViewer();
  OFString patientName;
  OFString instanceNumber;
  OFString acquisitionNumber;
  OFString transferSyntaxUID;
  OFString referencedFrameNumbers;  
  OFString patientSize;
  OFString bitsAllocated;
  OFString bitsStored; 
  OFString highBit;
  OFString windowCenter; 
  OFString windowWidth;
  OFString rescaleSlope;
  OFString rescaleIntercept;
  OFString energyWindowLowerLimit;
  OFString energyWindowUpperLimit;

public slots:
  void openDicom();
  void showStats();
};

#endif // DICOM_VIEWER_H
