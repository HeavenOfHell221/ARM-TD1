#include "dicom_viewer.h"
#include "dcmtk/dcmimage/diregist.h"

#include <iostream>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>

using namespace std;

DicomViewer::DicomViewer(QWidget *parent) : QMainWindow(parent)
{
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

void DicomViewer::openDicom()
{
	QString fileName_Qt = QFileDialog::getOpenFileName(this, tr("Open File"),"..",tr("Dcm File Format (*.dcm)"));
	
	if(fileName_Qt.isEmpty() || fileName_Qt.isNull()) 
	{
		// L'utilisateur n'a selectionné aucun fichier
		return;
	}
	
	string fileName_str = fileName_Qt.toStdString();
	const char* fileName = fileName_str.c_str();
	
	
	ostringstream msg_oss;
	DcmFileFormat fileformat;
	OFCondition status = fileformat.loadFile(fileName);

	if (status.good())
	{
		msg_oss << "Your file have been succesfully selected" << endl;
		DcmDataset *dataset = fileformat.getDataset();

		// (1/11) The dicom deftag DCM_PatientName seems to be the best
		if (dataset->findAndGetOFString(DCM_PatientName, patientName).good())
		{
			cout << "Patient's Name: " << patientName << endl;
		}
		else
		{
			cerr << "Error: cannot access Patient's Name." << endl;
		}

		/*-----------------------------*/

		// (2/11) The dicom deftag DCM_InstanceNumber seems to be the best
		if (dataset->findAndGetOFString(DCM_InstanceNumber, instanceNumber).good())
		{
			cout << "Instance number: " << instanceNumber << endl;
		}
		else
		{
			cerr << "Error: cannot access Instance number!" << endl;
		}

		/*-----------------------------*/

		// (3/11) The dicom deftag DCM_AcquisitionNumber seems to be the best
		if (dataset->findAndGetOFString(DCM_AcquisitionNumber, acquisitionNumber).good())
		{
			cout << "Acquisition number: " << acquisitionNumber << endl;
		}
		else
		{
			cerr << "Error: cannot access Acquisition number !" << endl;
		}

		/*-----------------------------*/

		// (4/11) The dicom deftag DCM_TransferSyntaxUID don't have the same label as required but seems to be
		//        the best among the others
		if (dataset->findAndGetOFString(DCM_TransferSyntaxUID, transferSyntaxUID).good())
		{
			cout << "Transfer Syntax UID: " << transferSyntaxUID << endl;
		}
		else
		{
			cerr << "Error: cannot access Transfer Syntax UID!" << endl;
		}

		/*-----------------------------*/

		// (5/11) The dicom deftag DCM_ReferencedFrameNumbers don't have the same label as required but seems to be
		//        the best among the others
		if (dataset->findAndGetOFString(DCM_ReferencedFrameNumbers, referencedFrameNumbers).good())
		{
			cout << "Referenced frame numbers: " << referencedFrameNumbers << endl;
		}
		else
		{
			cerr << "Error: cannot access Referenced frame numbers!" << endl;
		}

		/*-----------------------------*/

		// (6/11) The dicom deftag DCM_PatientSize don't have the same label as required but seems to be
		//        the best among the others
		if (dataset->findAndGetOFString(DCM_PatientSize, patientSize).good())
		{
			cout << "patientSize: " << patientSize << endl;
		}
		else
		{
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
		if (dataset->findAndGetOFString(DCM_BitsAllocated, bitsAllocated).good())
		{
			cout << "0,bitsAllocated: "
				 << "0"
				 << "," << bitsAllocated << endl;
		}
		else
		{
			cerr << "Error: cannot access bitsAllocated!" << endl;
		}

		/*-----------------------------*/

		// (8/11) Used values :
		//  - [0; 2^DCM_BitsStored]
		//  - [0; 2^DCM_HighBit]
		//  - [SmallestImagePixelValue; LargestImagePixelValue]
		if (dataset->findAndGetOFString(DCM_BitsStored, bitsStored).good() && dataset->findAndGetOFString(DCM_HighBit, highBit).good())
		{
			cout << "bitsStored,highBit: " << bitsStored << "," << highBit << endl;
		}
		else
		{
			cerr << "Error: cannot access bitsStored or highBit !" << endl;
		}

		/*-----------------------------*/

		// (9/11) Window :
		//  - [DCM_EnergyWindowLowerLimit; DCM_EnergyWindowUpperLimit]
		//  - Very likely : DCM_WindowCenter and DCM_WindowWidth
		if (dataset->findAndGetOFString(DCM_WindowCenter, windowCenter).good() && dataset->findAndGetOFString(DCM_WindowWidth, windowWidth).good())
		{
			cout << "windowCenter,windowWidth: " << windowCenter << "," << windowWidth << endl;
		}
		else
		{
			cerr << "Error: cannot access windowCenter or windowWidth !" << endl;
		}

		/*-----------------------------*/

		// (10/11) Slop : Maybe DCM_RescaleSlope
		if (dataset->findAndGetOFString(DCM_RescaleSlope, rescaleSlope).good())
		{
			cout << "rescaleSlope: " << rescaleSlope << endl;
		}
		else
		{
			cerr << "Error: cannot access rescaleSlope!" << endl;
		}

		/*-----------------------------*/

		//(11/11) Intercept : Maybe DCM_RescaleIntercept
		if (dataset->findAndGetOFString(DCM_RescaleIntercept, rescaleIntercept).good())
		{
			cout << "rescaleIntercept: " << rescaleIntercept << endl;
		}
		else
		{
			cerr << "Error: cannot access rescaleIntercept!" << endl;
		}

		/* ------------------------------ Part 3 : ------------------------------ */

		stringstream ss;
		int width, center, slope, intercept;
		ss << windowCenter;
		ss >> center;
		ss << windowWidth;
		ss >> width;
		ss << rescaleSlope;
		ss >> slope;
		ss << rescaleIntercept;
		ss >> intercept;

		DJDecoderRegistration::registerCodecs();
		DcmRLEDecoderRegistration::registerCodecs();

		DicomImage *image = new DicomImage(dataset, dataset->getCurrentXfer(), slope, intercept, 0, 0, 0);

		if (image != NULL)
		{
			image->setWindow(width, center);

			EI_Status status = image->getStatus();

			if (status == EIS_Normal)
			{	
				uchar* pixelData = (uchar*) (image->getOutputData(16));
				if(pixelData != NULL) {
					cerr << "Good" << endl;
					QImage *imageQt = new QImage(pixelData, image->getWidth(), image->getHeight(), QImage::Format_RGB16);
					cerr << imageQt->format() << endl;

					QLabel label;
					label.setPixmap(QPixmap::fromImage(*imageQt));
					label.show();
				}
				else {
					cerr << "Error getOutputData" << endl;
				}
				
			}
			else
			{
				cerr << "Error: cannot load DICOM image (" << DicomImage::getString(image->getStatus()) << ")" << endl;
			}
		}
		delete image;
	}
	else 
	{
		msg_oss << "Error." << endl;
	}
	QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}

void DicomViewer::showStats()
{
	ostringstream msg_oss;
	msg_oss << "Patient: " << patientName << endl;
	msg_oss << "Instance number: " << instanceNumber << endl;
	msg_oss << "Acquisition number: " << acquisitionNumber << endl;
	msg_oss << "Original transfer syntax: " << transferSyntaxUID << endl;
	msg_oss << "Nb frames: " << referencedFrameNumbers << endl;
	msg_oss << "Size: " << patientSize << endl;
	msg_oss << "Allowed values: "
			<< "["
			<< "0"
			<< "," << bitsAllocated << "]" << endl;
	msg_oss << "Used values: "
			<< "[" << bitsStored << "," << highBit << "]" << endl;
	msg_oss << "Window: "
			<< "[" << windowCenter << "," << windowWidth << "]" << endl;
	msg_oss << "Slope: " << rescaleSlope << endl;
	msg_oss << "Intercept: " << rescaleIntercept << endl;

	QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}
