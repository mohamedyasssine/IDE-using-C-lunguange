#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileInfoList>
#include <QListWidgetItem>
#include <QDir>
#include <QPushButton>
#include <QDesktopServices>
#include <QPlainTextEdit>
#include <QFileInfo>
#include <QFileDialog> // Pour la boîte de dialogue de sélection de fichiers

// Ajoutez cette directive d'inclusion pour utiliser QMessageBox
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Madi_X (v 1.10)");
    workspace="C:/one drive/Bureau/python/python";
    connect(ui->actionnouveau, &QAction::triggered, this, &MainWindow::on_actionnouveau_triggered);
    connect(ui->actioncharger_fichier, &QAction::triggered, this, &MainWindow::on_actioncharger_fichier_triggered);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &MainWindow::saveCurrentFile);
    connect(ui->actionchanger_workspace, &QAction::triggered, this, &MainWindow::on_actionchanger_workspace_triggered);
    connect(ui->actionenregistrer, &QAction::triggered, this, &MainWindow::saveCurrentFile);
    // Créez une icône pour représenter la fermeture des onglets
    connect(ui->tabWidget->tabBar(), &QTabBar::tabBarClicked, this, &MainWindow::addCloseIconToTab);
    // Connecter le signal tabCloseRequested à la fonction closeTab
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

}

void MainWindow::addCloseIconToTab(int index) {
    QIcon closeIcon("x.png");
    QPixmap pixmap = closeIcon.pixmap(QSize(3, 3)); // Redimensionnement de l'icône à la taille spécifiée (16x16 pixels)
    QIcon resizedIcon(pixmap); // Création d'une nouvelle icône redimensionnée à partir du pixmap
    ui->tabWidget->setTabIcon(index, closeIcon);

}
void MainWindow::closeTab(int index) {
    // Vérifiez si le fichier actuellement ouvert dans l'onglet est "united"
    if (ui->tabWidget->tabText(index) == tr("united")) {
        // Affichez une boîte de dialogue demandant à l'utilisateur s'il souhaite enregistrer le fichier
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Enregistrer fichier", "Voulez-vous enregistrer le fichier avant de fermer l'onglet?",
                                      QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes) {
            // Si l'utilisateur souhaite enregistrer le fichier, appelez la fonction saveCurrentFile()
            saveCurrentFile();
        } else if (reply == QMessageBox::Cancel) {
            // Si l'utilisateur annule la fermeture de l'onglet, ne faites rien
            return;
        }
    }

    // Fermez l'onglet
    ui->tabWidget->removeTab(index);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePlainTextEdit(int index)
{
    if (index >= 0) {
        // Récupérer le contenu de l'onglet sélectionné et le définir dans le QPlainTextEdit
        QPlainTextEdit *selectedTab = qobject_cast<QPlainTextEdit*>(ui->tabWidget->widget(index));
        if (selectedTab) {
            ui->plainTextEdit->setPlainText(selectedTab->toPlainText());
        }
    }
}

QString MainWindow::getFileContent(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return ""; // Gérer l'erreur si le fichier ne peut pas être ouvert
    }

    QTextStream in(&file);
    QString content = in.readAll();

    file.close();

    return content;
}

QString MainWindow::getWorkspace()
{
    return workspace;
}

void MainWindow::afficherWorkspace()
{
    QString workspace = getWorkspace();
    QDir directory(workspace);
    QFileInfoList fileList = directory.entryInfoList();

    // Effacer les onglets existants dans le QTabWidget (tabWidget)
    ui->tabWidget->clear();

    // Effacer les éléments existants dans la QListWidget (listWidget)
    ui->listWidget->clear();
    QStringList loadedFiles;


    // Ajouter uniquement les labels à la QListWidget (listWidget) lors du chargement initial
    for(const QFileInfo &fileInfo : fileList) {
        if(fileInfo.isFile()) {
            // Créer un nouvel élément QListWidgetItem
            QListWidgetItem *item = new QListWidgetItem();
            // Stocker le chemin complet du fichier en tant que données de l'élément
            item->setData(Qt::UserRole, fileInfo.filePath());
            // Créer un QLabel avec le nom du fichier
            QLabel *label = new QLabel(fileInfo.fileName());
            // Ajouter le QLabel à l'élément QListWidgetItem
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item, label);

        }
    }

    // Connecter le signal itemClicked() de la QListWidget à une fonction lambda
    connect(ui->listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        // Récupérer le chemin complet du fichier à partir des données de l'élément QListWidgetItem
        QString filePath = item->data(Qt::UserRole).toString();
        // Lire le contenu du fichier
        if (!isFileAlreadyLoaded(filePath)) {


            QString fileContent = getFileContent(filePath);
            // Ajouter le contenu du fichier à une nouvelle plainTextEdit
            QPlainTextEdit *fileTextEdit = new QPlainTextEdit();
            fileTextEdit->setPlainText(fileContent);
            // Ajouter un nouvel onglet dans le QTabWidget (tabWidget) avec la plainTextEdit associée
            ui->tabWidget->addTab(fileTextEdit, QFileInfo(filePath).fileName());
        }
    });
}

bool MainWindow::isFileAlreadyLoaded(const QString &filePath) const
{
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        QString tabText = ui->tabWidget->tabText(i);
        if (QFileInfo(filePath).fileName() == tabText) {
            return true; // Le fichier est déjà chargé dans un onglet
        }
    }
    return false; // Le fichier n'est pas déjà chargé dans un onglet
}

/*void MainWindow::on_actionnouveau_triggered()
{
    // Demander à l'utilisateur le nom du nouveau fichier
    QString fileName = QFileDialog::getSaveFileName(this, tr("Nouveau fichier"), workspace, tr("Fichiers texte (*.txt);;Tous les fichiers (*.*)"));

    // Vérifier si l'utilisateur a annulé la boîte de dialogue ou n'a pas entré de nom de fichier
    if (fileName.isEmpty()) {
        return; // Sortir de la fonction sans rien faire si aucun nom de fichier n'a été spécifié
    }

    // Vérifier si le fichier existe déjà
    QFileInfo fileInfo(fileName);
    if (fileInfo.exists()) {
        QMessageBox::warning(this, tr("Fichier existant"), tr("Le fichier existe déjà."));
        return; // Sortir de la fonction si le fichier existe déjà
    }

    // Créer un nouveau fichier avec le nom spécifié
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible de créer le fichier."));
        return; // Sortir de la fonction en cas d'erreur lors de la création du fichier
    }
    file.close();

    // Ajouter le nom du fichier à la liste (fileList)
    QLabel *label = new QLabel(fileInfo.fileName());
    QListWidgetItem *item = new QListWidgetItem();
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, label);

    // Ajouter le contenu du nouveau fichier dans un nouvel onglet du QTabWidget (tabWidget)
    QPlainTextEdit *fileContent = new QPlainTextEdit();
    fileContent->setPlainText(""); // Le nouveau fichier est vide pour le moment

    // Ajouter l'onglet avec le nouveau contenu
    ui->tabWidget->addTab(fileContent, fileInfo.fileName());

    // Sélectionner l'élément correspondant dans QListWidget
    ui->listWidget->setCurrentItem(item);

    // Définir l'onglet actif dans QTabWidget
    int index = ui->tabWidget->indexOf(fileContent);
    ui->tabWidget->setCurrentIndex(index);
}*/
void MainWindow::on_actionnouveau_triggered() {
    // Vérifier si une tab avec le nom "united" existe déjà
    bool unitedTabExists = false;
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        if (ui->tabWidget->tabText(i) == tr("united")) {
            unitedTabExists = true;
            break;
        }
    }

    // Si aucune tab "united" n'existe, en créer une nouvelle
    if (!unitedTabExists) {
        // Ajouter une nouvelle tab avec un QPlainTextEdit vide
        QPlainTextEdit *plainTextEdit = new QPlainTextEdit();
        ui->tabWidget->addTab(plainTextEdit, tr("united"));
        ui->tabWidget->setCurrentWidget(plainTextEdit);
    }
}
void MainWindow::on_actioncharger_fichier_triggered()
{
    // Demander à l'utilisateur de sélectionner un fichier existant
    QString filePath = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), workspace, tr("Fichiers texte (*.txt);;Tous les fichiers (*.*)"));

    // Vérifier si l'utilisateur a annulé la boîte de dialogue ou n'a pas sélectionné de fichier
    if (filePath.isEmpty()) {
        return; // Sortir de la fonction si aucun fichier n'a été sélectionné
    }

    // Créer un objet QFile avec le chemin du fichier sélectionné
    QFile file(filePath);

    // Vérifier si le fichier peut être ouvert en lecture
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le fichier."));
        return; // Sortir de la fonction en cas d'erreur lors de l'ouverture du fichier
    }

    // Lire le contenu du fichier
    QTextStream in(&file);
    QString fileContent = in.readAll();

    // Fermer le fichier
    file.close();

    // Ajouter le contenu du fichier dans un nouvel onglet du QTabWidget (tabWidget)
    QPlainTextEdit *fileTextEdit = new QPlainTextEdit();
    fileTextEdit->setPlainText(fileContent);

    // Extraire le nom du fichier à partir du chemin complet
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();

    // Ajouter l'onglet avec le contenu du fichier
    ui->tabWidget->addTab(fileTextEdit, fileName);

    // Sélectionner l'onglet nouvellement ajouté
    ui->tabWidget->setCurrentWidget(fileTextEdit);
}
/*void MainWindow::saveCurrentFile() {
    QString currentFileName = ui->tabWidget->tabText(ui->tabWidget->currentIndex()); // Obtenir le nom du fichier actuel
    QString currentFilePath = workspace + "/" + currentFileName; // Construire le chemin complet du fichier

    QFile file(currentFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << ui->plainTextEdit->toPlainText(); // Enregistrer le contenu actuel de plainTextEdit dans le fichier
        file.close();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible de sauvegarder le fichier."));
    }
}*/
void MainWindow::saveCurrentFile() {
    QString currentFileName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());

    // Vérifier si le fichier actuel est "united"
    if (currentFileName == tr("united")) {
        // Demander à l'utilisateur de choisir un nom de fichier
        QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), workspace, tr("Fichiers texte (*.txt);;Tous les fichiers (*.*)"));

        // Vérifier si l'utilisateur a annulé la boîte de dialogue ou n'a pas entré de nom de fichier
        if (fileName.isEmpty()) {
            return; // Sortir de la fonction sans rien faire si aucun nom de fichier n'a été spécifié
        }

        // Mise à jour du nom de l'onglet avec le nom de fichier choisi
        int currentIndex = ui->tabWidget->currentIndex();
        ui->tabWidget->setTabText(currentIndex, QFileInfo(fileName).fileName());

        // Mise à jour du chemin du fichier actuel
        currentFileName = fileName;

        // Ajouter ce return pour sortir de la fonction une fois le nom de l'onglet mis à jour
        return;
    }

    // Construire le chemin complet du fichier
    QString currentFilePath = workspace + "/" + currentFileName;

    // Écrire le contenu du QPlainTextEdit dans le fichier
    QFile file(currentFilePath);
    //Open the file
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    out << ui->plainTextEdit->toPlainText() << "\n";

    file.close();
}


void MainWindow::on_actionchanger_workspace_triggered()
{
    // Demander à l'utilisateur de choisir un nouveau répertoire de travail
    QString newWorkspace = QFileDialog::getExistingDirectory(this, tr("Changer le répertoire de travail"), workspace);

    // Vérifier si l'utilisateur a annulé la sélection ou n'a pas choisi de nouveau répertoire
    if (newWorkspace.isEmpty()) {
        return; // Sortir de la fonction si aucun nouveau répertoire n'a été sélectionné
    }

    // Mettre à jour le répertoire de travail avec le nouveau chemin
    workspace = newWorkspace;
    ui->listWidget->clear();

    // Afficher les fichiers du nouveau répertoire de travail dans la QListWidget et le QTabWidget
    afficherWorkspace();
}
