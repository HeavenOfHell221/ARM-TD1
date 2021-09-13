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

void DicomViewer::openDicom() {}

void DicomViewer::showStats() {
  std::ostringstream msg_oss;
  msg_oss << "Patient: TODO" << std::endl;
  msg_oss << "Instance number: TODO" << std::endl;
  msg_oss << "Acquisition number: TODO" << std::endl;
  msg_oss << "Original transfer syntax: TODO" << std::endl;
  msg_oss << "Nb frames: TODO" << std::endl;
  msg_oss << "Size: W*H*D" << std::endl;
  msg_oss << "Allowed values: [TODO,TODO]" << std::endl;
  msg_oss << "Used values: [TODO,TODO]" << std::endl;
  msg_oss << "Window: [TODO,TODO]" << std::endl;
  msg_oss << "Slope: TODO" << std::endl;
  msg_oss << "Intercept: TODO" << std::endl;
  QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}
