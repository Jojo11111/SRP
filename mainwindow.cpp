#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    ui->qvtkWidgetAxl->GetRenderWindow()->AddRenderer(renderer);
    ui->qvtkWidgetCor->GetRenderWindow()->AddRenderer(renderer);
    ui->qvtkWidgetSag->GetRenderWindow()->AddRenderer(renderer);
    ui->widget_4->GetRenderWindow()->AddRenderer(renderer);
}

MainWindow::~MainWindow()
{
    //delete image;
    delete ui;
}

void MainWindow::on_actionADjust_InitUI_triggered()
{
    InitDialog *initdialog=new InitDialog(this);
    initdialog->exec () ;// 以模态方式显示对话框
}

void MainWindow::openDICOMFolder() {
    folderNameDICOM = QFileDialog::getExistingDirectory(this, tr("Open DICOM Image/Folder"), "/", QFileDialog::ShowDirsOnly);
    if(!folderNameDICOM.isEmpty()) {
        initialization();
        drawDICOMSeries(folderNameDICOM.toUtf8().constData());
    }
}

void MainWindow::initialization() {
    //Initializaton
    viewerAxl = vtkSmartPointer<vtkImageViewer2>::New();
    viewerCor = vtkSmartPointer<vtkImageViewer2>::New();
    viewerSag = vtkSmartPointer<vtkImageViewer2>::New();
    gdcmIO = ImageIOType::New();
    inputNames = InputNamesGeneratorType::New();
    reader = ReaderType::New();
    connector= ConnectorType::New();
    image = vtkImageData::New();
    activeCam = vtkCamera::New();
}

void MainWindow::drawDICOMSeries(std::string folderDICOM)
{
    //Read the input series
    inputNames->SetInputDirectory(folderDICOM.c_str());
    const ReaderType::FileNamesContainer & filenames = inputNames->GetInputFileNames();
    reader->SetImageIO(gdcmIO);
    reader->SetFileNames(filenames);
    try {
        reader->Update();
    } catch (itk::ExceptionObject &excp) {
        std::cerr << "Exception thrown while reading the series" << std::endl;
        std::cerr << excp << std::endl;
    }

    //Connector to convert ITK image data to VTK image data
    connector->SetInput(reader->GetOutput()); //Set ITK reader Output to connector you can replace it with filter
    connector->Update();
    try { //Exceptional handling
        connector->Update();
    } catch (itk::ExceptionObject & e) {
        std::cerr << "exception in file reader " << std::endl;
        std::cerr << e << std::endl;
    }

    //Deep copy connector's output to an image else connector will go out of scope and vanish it will cause error while changing slice
    image->DeepCopy(connector->GetOutput());

    //Set VTK Viewer to QVTKWidget
    showAxl();
    showCor();
    showSag();
}

void MainWindow::showAxl() {
    //Set input image to VTK viewer
    viewerAxl->SetInputData(image);

    //Flipping Y-axis (further checking needed)
    cam = viewerAxl->GetRenderer()->GetActiveCamera();
    cam->SetPosition(0,0,-1);
    cam->SetViewUp(0,-1,0);

    //ui->qvtkWidgetAxl->SetRenderWindow(viewerAxl->GetRenderWindow());
    viewerAxl->SetRenderWindow(ui->qvtkWidgetAxl->GetRenderWindow());

    viewerAxl->SetupInteractor(ui->qvtkWidgetAxl->GetRenderWindow()->GetInteractor());
    viewerAxl->SetSliceOrientationToXY();
    //viewerAxl->Render();
    ui->qvtkWidgetAxl->GetRenderWindow()->Render();

    // Gets the number of slices
    minSliceAxl = viewerAxl->GetSliceMin();
    maxSliceAxl = viewerAxl->GetSliceMax();
    viewerAxl->SetSlice(viewerAxl->GetSliceMax()/2);
    viewerAxl->GetRenderer()->ResetCamera();
    viewerAxl->Render();
    ui->qvtkWidgetAxl->update();
}

void MainWindow::showCor() {
    //Set input image to VTK viewer
    viewerCor->SetInputData(image);

    //ui->qvtkWidgetCor->SetRenderWindow(viewerCor->GetRenderWindow());
    viewerCor->SetRenderWindow(ui->qvtkWidgetCor->GetRenderWindow());
    viewerCor->SetupInteractor(ui->qvtkWidgetCor->GetRenderWindow()->GetInteractor());
    viewerCor->SetSliceOrientationToXZ();
     ui->qvtkWidgetCor->GetRenderWindow()->Render();

    // Gets the number of slices
    minSliceCor = viewerCor->GetSliceMin();
    maxSliceCor = viewerCor->GetSliceMax();
    viewerCor->SetSlice(viewerCor->GetSliceMax()/2);
    viewerCor->GetRenderer()->ResetCamera();
    viewerCor->Render();
    ui->qvtkWidgetCor->update();
}

void MainWindow::showSag() {
    //Set input image to VTK viewer
    viewerSag->SetInputData(image);

    //ui->qvtkWidgetSag->SetRenderWindow(viewerSag->GetRenderWindow());
    viewerSag->SetRenderWindow(ui->qvtkWidgetSag->GetRenderWindow());
    viewerSag->SetupInteractor(ui->qvtkWidgetSag->GetRenderWindow()->GetInteractor());
    viewerSag->SetSliceOrientationToYZ();
     ui->qvtkWidgetSag->GetRenderWindow()->Render();

    // Gets the number of slices
    minSliceSag = viewerSag->GetSliceMin();
    maxSliceSag = viewerSag->GetSliceMax();
    viewerSag->SetSlice(viewerSag->GetSliceMax()/2);
    viewerSag->GetRenderer()->ResetCamera();
    viewerSag->Render();
    ui->qvtkWidgetSag->update();
}

void MainWindow::on_actionOpen_triggered()
{
    openDICOMFolder();
}

void MainWindow::on_actionOpen_2_triggered()
{
    openDICOMFolder();
}

void MainWindow::on_actionExit_triggered()
{
     QApplication::quit();
}

void MainWindow::on_actionClose_triggered()
{
    QApplication::closeAllWindows();
}
