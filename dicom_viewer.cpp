#include "dicom_viewer.h"
#include "dcmtk/dcmimage/diregist.h"

#include <iostream>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <ctype.h> //Check the belonging to a type

using namespace std;

DicomViewer::DicomViewer(QWidget *parent) : QMainWindow(parent), label(new QLabel)
{
    // Setting menu
    setCentralWidget(label);
    QMenu *file_menu = menuBar()->addMenu("&File");
    QAction *open_action = file_menu->addAction("&Open");
    open_action->setShortcut(QKeySequence::Open);
    QObject::connect(open_action, SIGNAL(triggered()), this, SLOT(openDicom()));
    QAction *help_action = file_menu->addAction("&Help");
    help_action->setShortcut(QKeySequence::HelpContents);
    QObject::connect(help_action, SIGNAL(triggered()), this, SLOT(showStats()));
}

DicomViewer::~DicomViewer()
{
    delete image;
}

void DicomViewer::openDicom()
{
    QString fileName_Qt = QFileDialog::getOpenFileName(this, tr("Open File"), "..", tr("Dcm File Format (*.dcm)"));

    if (fileName_Qt.isEmpty() || fileName_Qt.isNull())
    {
        // L'utilisateur n'a selectionn√© aucun fichier
        return;
    }

    string fileName_str = fileName_Qt.toStdString();
    const char *fileName = fileName_str.c_str();

    ostringstream msg_oss;
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(fileName);

    if (status.good())
    {
        DcmDataset *dataset = fileformat.getDataset();
        dataset->findAndGetOFString(DCM_PatientName, patientName);
        dataset->findAndGetOFString(DCM_InstanceNumber, instanceNumber);
        dataset->findAndGetOFString(DCM_AcquisitionNumber, acquisitionNumber);
        dataset->findAndGetOFString(DCM_TransferSyntaxUID, transferSyntaxUID);
        dataset->findAndGetOFString(DCM_ReferencedFrameNumbers, referencedFrameNumbers);
        dataset->findAndGetOFString(DCM_PatientSize, patientSize);
        dataset->findAndGetOFString(DCM_BitsAllocated, bitsAllocated);
        dataset->findAndGetOFString(DCM_MinimumStoredValueMapped, minimumStoredValueMapped);
        dataset->findAndGetOFString(DCM_MaximumStoredValueMapped, maximumStoredValueMapped);
        dataset->findAndGetOFString(DCM_EnergyWindowLowerLimit, energyWindowLowerLimit);
        dataset->findAndGetOFString(DCM_EnergyWindowUpperLimit, energyWindowUpperLimit);
        dataset->findAndGetOFString(DCM_RescaleSlope, rescaleSlope);
        dataset->findAndGetOFString(DCM_RescaleIntercept, rescaleIntercept);
        dataset->findAndGetOFString(DCM_WindowCenter, windowCenter);
        dataset->findAndGetOFString(DCM_WindowWidth, windowWidth);

        /* ------------------------------ Part 3 : Start ------------------------------ */

        int width, center, slope, intercept;
        width = atoi(windowWidth.c_str());
        center = atoi(windowCenter.c_str());
        slope = atoi(rescaleSlope.c_str());
        intercept = atoi(rescaleIntercept.c_str());

        DJDecoderRegistration::registerCodecs();
        DcmRLEDecoderRegistration::registerCodecs();

        image = new DicomImage(dataset, dataset->getCurrentXfer(), slope, intercept, 0, 0, 0);

        image->setWindow(width, center);
        image->setMinMaxWindow(1);

        if (image != NULL)
        {
            EI_Status status = image->getStatus();

            if (status == EIS_Normal)
            {
                Uint8 *pixelData = (Uint8 *)(image->getOutputData(8));
                if (pixelData != NULL)
                {
                    QImage *imageQt = new QImage(pixelData, image->getWidth(), image->getHeight(), QImage::Format_Indexed8);
                    label->setPixmap(QPixmap::fromImage(*imageQt));

                    msg_oss << "Succes" << endl;
                }
                else
                {
                    msg_oss << "Error with getOutputData" << endl;
                }
            }
            else
            {
                msg_oss << "Error: cannot load DICOM image (" << DicomImage::getString(image->getStatus()) << ")" << endl;
            }
        }

        /* ------------------------------ Part 3 : End ------------------------------ */
    }
    else
    {
        cerr << "Error: cannot read DICOM file (" << status.text() << ")" << endl;
    }

    QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}

bool isNumber(const string &str) {
    const char *cha = str.c_str();
    if (strcmp(cha, "-") == 0 || std::isdigit(str[0]) != 0)
    {
        for (char const &c : str)
        {
            if (std::isdigit(c) == 0)
                return false;
        }
    }
    return true;
}

void DicomViewer::showStats()
{
    if (image == NULL)
    {
        return;
    }

    ostringstream msg_oss;
    DcmFileFormat fileformat;

    // (1/11) The dicom deftag DCM_PatientName seems to be the best
    // [DONE] - Not an empty string
    if (patientName.empty())
    {
        msg_oss << "Patient name information is empty !" << endl;
    }
    else
    {
        msg_oss << "Patient: " << patientName << endl;
    }

    // (2/11) The dicom deftag DCM_InstanceNumber seems to be the best
    // [DONE] - Not an empty string
    // [DONE] - Check if it's a positive or negative number
    if (instanceNumber.empty())
    {
        msg_oss << "Instance number information is empty !" << endl;
    }
    else if (!isNumber(instanceNumber.c_str()))
    {
        msg_oss << "Can't detect a valid instance number !" << endl;
    }
    else
    {
        msg_oss << "Instance number: " << instanceNumber << endl;
    }

    // (3/11) The dicom deftag DCM_AcquisitionNumber seems to be the best
    // [DONE] - Not an empty string
    // [DONE] - Check if it's a positive or negative number
    if (acquisitionNumber.empty())
    {
        msg_oss << "Acquisition number information is empty !" << endl;
    }
    else if (!isNumber(acquisitionNumber.c_str()))
    {
        msg_oss << "Can't detect a valid acquisition number !" << endl;
    }
    else
    {
        msg_oss << "Acquisition number: " << acquisitionNumber << endl;
    }

    // (4/11) The dicom deftag DCM_TransferSyntaxUID don't have the same label as required but seems to be
    //        the best among the others
    // [DONE] - Not an empty string
    if (transferSyntaxUID.empty())
    {
        msg_oss << "transferSyntaxUID information is empty !" << endl;
    }
    else
    {
        msg_oss << "Original transfer syntax: " << transferSyntaxUID << endl;
    }

    // (5/11) The dicom deftag DCM_ReferencedFrameNumbers don't have the same label as required but seems to be
    //        the best among the others
    // [DONE] - Not an empty string
    // [DONE] - Check if it's a positive or negative number
    if (referencedFrameNumbers.empty())
    {
        msg_oss << "ReferencedFrameNumbers information is empty !" << endl;
    }
    else if (!isNumber(referencedFrameNumbers.c_str()))
    {
        msg_oss << "Can't detect a valid referencedFrameNumbers !" << endl;
    }
    else
    {
        msg_oss << "Nb frames: " << referencedFrameNumbers << endl;
    }

    /*** ---------- TODO : START ----------***/
    // (6/11) The dicom deftag DCM_PatientSize don't have the same label as required but seems to be
    //        the best among the others
    // [TODO] - Not an empty string
    // [TODO] - Check if it's a triplet of positive or negative number
    msg_oss << "Size: " << image->getWidth() << "*" << image->getHeight() << "*" << image->getDepth() << endl;

    // (7/11) Allowed values : If it's a range
    //  - DCM_NominalMinEnergy / DCM_NominalMaxEnergy
    //  - DCM_MinimumStoredValueMapped / DCM_MaximumStoredValueMapped
    //  - DCM_MinDensity / DCM_MaxDensity
    //  - DCM_DVHMinimumDose / DCM_DVHMaximumDose
    //  - DCM_ChannelMinimumValue / DCM_ChannelMaximumValue
    //  There is too many possibilities to choose randomly one among them, so it's possibly none of them and more likely
    //  a range between 0 and 2^DCM_BitsAllocated.
    // [TODO] - Not an empty string
    // [TODO] - Check if it's a positive or negative number
    // [TODO] - Check if the min value is lower than the max value
    // [TODO] - Find the thing linked to bitsAllocated
    double min, max = 0;
    image->getMinMaxValues(min, max);
    if (min > max)
    {
        msg_oss << "min is over than the max !" << endl;
    }
    msg_oss << "Allowed values: "
            << "[min:" << min << ", max:" << max << "]" << endl;

    // (8/11) Used values :
    // [DONE] - Not an empty string
    // [DONE] - Check if it's a doublet of positive or negative number
    // [DONE] - Check if the min value is lower than the max value
    // [DONE] - Find the array to browse
    if (minimumStoredValueMapped.empty())
    {
        msg_oss << "minimumStoredValueMapped information is empty !" << endl;
    }
    else if (maximumStoredValueMapped.empty())
    {
        msg_oss << "maximumStoredValueMapped information is empty !" << endl;
    }
    else if (!isNumber(minimumStoredValueMapped.c_str()))
    {
        msg_oss << "Can't detect a valid minimumStoredValueMapped !" << endl;
    }
    else if (!isNumber(maximumStoredValueMapped.c_str()))
    {
        msg_oss << "Can't detect a valid maximumStoredValueMapped !" << endl;
    }
    else
    {
        double lower = atoi(minimumStoredValueMapped.c_str());
        double upper = atoi(maximumStoredValueMapped.c_str());

        if (lower > upper)
        {
            msg_oss << "Lower value is over than the Upper value !" << endl;
        }
        else
        {
            msg_oss << "Used values: "
                    << "[min:" << minimumStoredValueMapped << ", max:" << maximumStoredValueMapped << "]" << endl;
        }
    }

    /*** ---------- TODO : END ----------***/

    // (9/11) The dicom deftags DCM_EnergyWindowLowerLimit and DCM_EnergyWindowUpperLimit seem to be the bests
    // [DONE] - Not an empty string
    // [DONE] - Check if it's a doublet of positive or negative number
    // [DONE] - Check if the min value is lower than the max value
    if (energyWindowLowerLimit.empty())
    {
        msg_oss << "energyWindowLowerLimit information is empty !" << endl;
    }
    else if (energyWindowUpperLimit.empty())
    {
        msg_oss << "energyWindowUpperLimit information is empty !" << endl;
    }
    else if (!isNumber(energyWindowLowerLimit.c_str()))
    {
        msg_oss << "Can't detect a valid energyWindowLowerLimit !" << endl;
    }
    else if (!isNumber(energyWindowUpperLimit.c_str()))
    {
        msg_oss << "Can't detect a valid energyWindowUpperLimit !" << endl;
    }
    else
    {
        double lower = atoi(energyWindowLowerLimit.c_str());
        double upper = atoi(energyWindowUpperLimit.c_str());

        if (lower > upper)
        {
            msg_oss << "Lower limit is over than the Upper limit !" << endl;
        }
        else
        {
            msg_oss << "Window: "
                    << "[min:" << energyWindowLowerLimit << ", max:" << energyWindowUpperLimit << "]" << endl;
        }
    }

    // (10/11) The dicom deftag DCM_RescaleSlope seems to be the best
    // [DONE] - Not an empty string
    // [DONE] - Check if it's a positive or negative number
    if (rescaleSlope.empty())
    {
        msg_oss << "Slope information is empty !" << endl;
    }
    else if (!isNumber(rescaleSlope.c_str()))
    {
        msg_oss << "Can't detect a valid Slope !" << endl;
    }
    else
    {
        msg_oss << "Slope: " << rescaleSlope << endl;
    }

    //(11/11) The dicom deftag DCM_RescaleIntercept seems to be the best
    // [DONE] - Not an empty string
    // [DONE] - Check if it's a positive or negative number
    if (rescaleIntercept.empty())
    {
        msg_oss << "Intercept information is empty !" << endl;
    }
    else if (!isNumber(rescaleIntercept.c_str()))
    {
        msg_oss << "Can't detect a valid Intercept !" << endl;
    }
    else
    {
        msg_oss << "Intercept: " << rescaleIntercept << endl;
    }

    QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}
