#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_stagiaireWidget(nullptr)
    , m_coursWidget(nullptr)
    , m_inscriptionWidget(nullptr)
{
    ui->setupUi(this);

    // ── Widgets modules ──────────────────────────────────────────
    m_stagiaireWidget   = new StagiaireWidget(this);
    m_coursWidget       = new CoursWidget(this);
    m_inscriptionWidget = new InscriptionWidget(this);

    ui->stackedWidget_2->addWidget(m_stagiaireWidget);
    ui->stackedWidget_2->addWidget(m_coursWidget);
    ui->stackedWidget_2->addWidget(m_inscriptionWidget);

    // ── Connexions boutons ───────────────────────────────────────
    connect(ui->pushButton_4, &QPushButton::clicked,
            this, &MainWindow::onStagiairesClicked);
    connect(ui->pushButton_3, &QPushButton::clicked,
            this, &MainWindow::onCoursClicked);
    connect(ui->pushButton_5, &QPushButton::clicked,
            this, &MainWindow::onInscriptionsClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ─── En-tête ─────────────────────────────────────────────────────────────────

void MainWindow::setHeader(const QString &titre, const QString &sousTitre,
                           const QString &couleur)
{
    ui->labelHeaderTitre->setText(titre);
    ui->labelHeaderTitre->setStyleSheet(
        QString("color: %1; font: bold 18pt 'Segoe UI';"
                " background: transparent; border: none;").arg(couleur));
    ui->labelHeaderSousTitre->setText(sousTitre);
}

// ─── Slots ───────────────────────────────────────────────────────────────────

void MainWindow::onStagiairesClicked()
{
    setHeader("Gestion des Stagiaires",
              "Gérez les stagiaires de votre centre de formation",
              "#89b4fa");
    ui->stackedWidget_2->setCurrentWidget(m_stagiaireWidget);
}

void MainWindow::onCoursClicked()
{
    setHeader("Gestion des Cours",
              "Gérez les cours et les inscriptions de votre centre",
              "#cba6f7");
    ui->stackedWidget_2->setCurrentWidget(m_coursWidget);
}

void MainWindow::onInscriptionsClicked()
{
    setHeader("Gestion des Inscriptions",
              "Inscrivez les stagiaires aux cours et suivez leur statut",
              "#fab387");
    ui->stackedWidget_2->setCurrentWidget(m_inscriptionWidget);
}
