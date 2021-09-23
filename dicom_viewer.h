#ifndef DICOM_VIEWER_H
#define DICOM_VIEWER_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>

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
  OFString minimumStoredValueMapped; 
  OFString maximumStoredValueMapped;
  OFString windowCenter; 
  OFString windowWidth;
  OFString rescaleSlope;
  OFString rescaleIntercept;
  OFString energyWindowLowerLimit;
  OFString energyWindowUpperLimit;
  QLabel *label;
  DicomImage *image;

public slots:
  void openDicom();
  void showStats();
};

#endif // DICOM_VIEWER_H
