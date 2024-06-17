#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString getWorkspace();
    void afficherWorkspace();
    void updatePlainTextEdit(int index);
    QString getFileContent(const QString &filePath);

private slots:
    void on_actionnouveau_triggered(); // Déclaration du slot pour gérer le signal triggered() du QAction
    void on_actioncharger_fichier_triggered();
    void saveCurrentFile();
    void on_actionchanger_workspace_triggered();
    void addCloseIconToTab(int index);
    void closeTab(int index);
private:
    Ui::MainWindow *ui;
    QString workspace;
    bool isFileAlreadyLoaded(const QString &fileName) const;


};
#endif // MAINWINDOW_H
