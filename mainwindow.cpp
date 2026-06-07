#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>      // Ventanas emergentes de alerta e información
#include <QFileDialog>      // Diálogo para seleccionar ruta al guardar archivos
#include <QNetworkRequest>  // Configuración de solicitudes HTTP
#include <QNetworkReply>    // Respuesta de solicitudes HTTP
#include <QUrl>             // Manejo y codificación de URLs
#include <QLabel>           // Widget para mostrar texto e imágenes
#include <QHBoxLayout>      // Layout horizontal para el historial
#include <QApplication>     // Necesario para qApp (aplicar estilos globales)
#include "infousuariowindow.h"

/*
 * Constructor de MainWindow.
 * Recibe un puntero a Usuario (puede ser UsuarioComun o Administrador).
 * Gracias al polimorfismo, la interfaz se adapta automáticamente
 * según el tipo de usuario sin necesidad de condicionales adicionales.
 */
MainWindow::MainWindow(Usuario *usuario, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , usuarioActual(usuario)
    , temaOscuro(true)          // El tema oscuro es el predeterminado
{
    ui->setupUi(this);

    // El título muestra el tipo y nombre del usuario autenticado (polimorfismo en acción)
    setWindowTitle("Generador de Imágenes IA - " + usuario->getTipo() + ": " + usuario->getNombre());
    setMinimumSize(800, 600);

    // QNetworkAccessManager gestiona las peticiones HTTP a la API de imágenes
    manager = new QNetworkAccessManager(this);
    // Signal & Slot: cuando la descarga termina, se llama a onImagenDescargada
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::onImagenDescargada);

    ui->campoDescripcion->setPlaceholderText("Describe the image in English...");

    // Carga los estilos artísticos disponibles en el selector
    ui->selectorEstilo->addItem("Ghibli");
    ui->selectorEstilo->addItem("Pixar");
    ui->selectorEstilo->addItem("Art Nouveau");
    ui->selectorEstilo->addItem("Moebius");

    ui->labelImagen->setAlignment(Qt::AlignCenter);
    ui->labelImagen->setFixedHeight(400);

    // Guardar deshabilitado hasta que haya una imagen generada
    ui->botonGuardar->setEnabled(false);

    // --- Configuración del historial de imágenes ---
    // Se usa un QWidget con layout horizontal dentro de un QScrollArea
    widgetHistorial = new QWidget();
    layoutHistorial = new QHBoxLayout(widgetHistorial);
    layoutHistorial->setAlignment(Qt::AlignLeft);
    ui->areaHistorial->setWidget(widgetHistorial);
    ui->areaHistorial->setWidgetResizable(true);
    ui->areaHistorial->setFixedHeight(120);
    ui->areaHistorial->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->areaHistorial->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // El historial solo es visible para administradores (uso de polimorfismo con esAdmin())
    ui->areaHistorial->setVisible(usuarioActual->esAdmin());
    ui->labelHistorial->setVisible(usuarioActual->esAdmin());

    // --- Conexión de signals & slots para cada botón ---
    connect(ui->botonGenerar, &QPushButton::clicked, this, &MainWindow::onGenerarClicked);
    connect(ui->botonGuardar, &QPushButton::clicked, this, &MainWindow::onGuardarClicked);
    connect(ui->botonBorrarHistorial, &QPushButton::clicked, this, &MainWindow::onBorrarHistorialClicked);

    // Borrar historial: solo visible para administradores
    ui->botonBorrarHistorial->setVisible(usuarioActual->esAdmin());

    connect(ui->botonInfoUsuario, &QPushButton::clicked, this, &MainWindow::onInfoUsuarioClicked);

    // Cambio de tema: funcionalidad exclusiva del administrador
    ui->botonToggleTema->setVisible(usuarioActual->esAdmin());
    connect(ui->botonToggleTema, &QPushButton::clicked, this, &MainWindow::onToggleTemaClicked);

    ui->botonCerrarSesion->setStyleSheet("background-color: #c0392b; color: white;");
    connect(ui->botonCerrarSesion, &QPushButton::clicked, this, &QMainWindow::close);

    // Aplica el tema visual inicial (oscuro por defecto)
    aplicarTema();
}

// Destructor: Qt libera los widgets hijos automáticamente,
// pero el UI generado por Designer debe liberarse manualmente
MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * Detecta si el texto contiene palabras comunes en español.
 * Se usa para alertar al usuario si escribe el prompt en el idioma incorrecto.
 * La API de imágenes requiere descripciones en inglés para mejores resultados.
 */
bool MainWindow::contieneEspanol(const QString &texto)
{
    QStringList palabrasEspanol = {
        "una", "un", "el", "la", "los", "las", "de", "en", "con",
        "por", "para", "que", "es", "son", "está", "están",
        "ciudad", "futurista", "iluminada", "perro", "gato",
        "casa", "árbol", "cielo", "mar", "montaña"
    };

    QString textoLower = texto.toLower();
    for (const QString &palabra : palabrasEspanol) {
        if (textoLower.split(" ").contains(palabra)) {
            return true;
        }
    }
    return false;
}

/*
 * Construye el prompt final combinando la descripción del usuario
 * con el modificador de estilo artístico seleccionado.
 * Este prompt se envía como parte de la URL a la API de Pollinations.
 */
QString MainWindow::construirPrompt(const QString &descripcion, const QString &estilo)
{
    QString modificador;
    if (estilo == "Ghibli")
        modificador = "in the style of Studio Ghibli, anime, soft watercolor, miyazaki";
    else if (estilo == "Pixar")
        modificador = "in the style of Pixar 3D animation, vibrant colors, detailed";
    else if (estilo == "Art Nouveau")
        modificador = "in the style of Art Nouveau, ornate, flowing lines, decorative";
    else if (estilo == "Moebius")
        modificador = "in the style of Moebius comic art, detailed linework, surreal sci-fi";

    return descripcion + ", " + modificador;
}

/*
 * Slot: se ejecuta al presionar "Generar".
 * Valida el límite de imágenes (polimorfismo con puedeGenerar()),
 * valida el idioma del prompt, y lanza la petición HTTP a la API.
 */
void MainWindow::onGenerarClicked()
{
    // Consulta polimórfica: UsuarioComun tiene límite 5, Administrador no tiene límite
    if (!usuarioActual->puedeGenerar()) {
        QMessageBox::warning(this, "Límite alcanzado",
                             QString("Alcanzaste el límite de %1 imágenes por sesión.")
                                 .arg(usuarioActual->getLimite()));
        ui->botonGenerar->setEnabled(false);
        ui->botonGenerar->setText("Límite alcanzado");
        return;
    }

    QString descripcion = ui->campoDescripcion->text().trimmed();
    if (descripcion.isEmpty()) {
        QMessageBox::warning(this, "Atención", "Escribí una descripción primero.");
        return;
    }

    // Validación de idioma: la API funciona mejor con prompts en inglés
    if (contieneEspanol(descripcion)) {
        QMessageBox::warning(this, "Idioma no soportado",
                             "El prompt debe estar en inglés.\n"
                             "Por ejemplo: 'a futuristic city with neon lights'");
        return;
    }

    QString estilo = ui->selectorEstilo->currentText();
    QString prompt = construirPrompt(descripcion, estilo);

    // Deshabilitar el botón mientras se procesa para evitar requests duplicadas
    ui->botonGenerar->setEnabled(false);
    ui->botonGenerar->setText("Generando...");
    ui->labelImagen->setText("Generando imagen, esperá un momento...");

    // Codifica el prompt para incluirlo en la URL (reemplaza espacios y caracteres especiales)
    QString promptEncoded = QString::fromUtf8(QUrl::toPercentEncoding(prompt));
    QUrl url("https://image.pollinations.ai/prompt/" + promptEncoded + "?width=512&height=512&nologo=true&model=turbo");
    QNetworkRequest request;
    request.setUrl(url);

    // Envía la petición GET — la respuesta llega al slot onImagenDescargada
    manager->get(request);
}

/*
 * Slot: se ejecuta automáticamente cuando la API responde.
 * Procesa la imagen recibida, actualiza el contador y el historial.
 */
void MainWindow::onImagenDescargada(QNetworkReply *reply)
{
    // Rehabilita el botón solo si el usuario todavía puede generar más imágenes
    if (usuarioActual->puedeGenerar()) {
        ui->botonGenerar->setEnabled(true);
        ui->botonGenerar->setText("Generar");
    }

    // Manejo de error HTTP: muestra el mensaje y cancela el procesamiento
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Error", "No se pudo generar la imagen:\n" + reply->errorString());
        reply->deleteLater();
        return;
    }

    // Lee los bytes de la respuesta y los convierte en imagen
    QByteArray datos = reply->readAll();
    pixmapActual.loadFromData(datos);

    // Escala la imagen al tamaño del label manteniendo proporciones
    QPixmap escalada = pixmapActual.scaled(ui->labelImagen->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelImagen->setPixmap(escalada);
    ui->botonGuardar->setEnabled(true);

    // Incrementa el contador de imágenes generadas en la sesión
    usuarioActual->incrementarContador();

    // Si tras incrementar se alcanzó el límite, bloquea el botón inmediatamente
    if (!usuarioActual->puedeGenerar()) {
        ui->botonGenerar->setEnabled(false);
        ui->botonGenerar->setText("Límite alcanzado");
    }

    // Agrega miniatura al historial — funcionalidad exclusiva del administrador
    if (usuarioActual->esAdmin()) {
        agregarAlHistorial(pixmapActual);
    }

    // Libera la memoria de la respuesta HTTP
    reply->deleteLater();
}

/*
 * Slot: abre un diálogo para que el usuario elija dónde guardar la imagen.
 * Soporta formato PNG y JPG.
 */
void MainWindow::onGuardarClicked()
{
    if (pixmapActual.isNull()) return;

    QString ruta = QFileDialog::getSaveFileName(this, "Guardar Imagen", "", "Imágenes (*.png *.jpg)");
    if (!ruta.isEmpty()) {
        pixmapActual.save(ruta);
        QMessageBox::information(this, "Guardado", "Imagen guardada correctamente.");
    }
}

/*
 * Slot: elimina todas las miniaturas del historial.
 * Recorre el layout y libera cada widget e item de memoria.
 * Solo accesible para administradores.
 */
void MainWindow::onBorrarHistorialClicked()
{
    QLayoutItem *item;
    while ((item = layoutHistorial->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    QMessageBox::information(this, "Historial", "Historial borrado correctamente.");
}

/*
 * Agrega una miniatura de 100x100 píxeles al historial.
 * Se llama cada vez que el administrador genera una imagen exitosamente.
 */
void MainWindow::agregarAlHistorial(const QPixmap &pixmap)
{
    QLabel *miniatura = new QLabel();
    miniatura->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    miniatura->setFixedSize(100, 100);
    layoutHistorial->addWidget(miniatura);
}

/*
 * Slot: abre la ventana de información del usuario como diálogo modal.
 * Muestra nombre, tipo de cuenta e imágenes generadas en la sesión.
 * Usa exec() para bloquear la ventana principal hasta que se cierre.
 */
void MainWindow::onInfoUsuarioClicked()
{
    InfoUsuarioWindow *ventanaInfo = new InfoUsuarioWindow(usuarioActual, this);
    ventanaInfo->exec();
}

/*
 * Aplica el stylesheet correspondiente al tema actual en toda la aplicación.
 * qApp->setStyleSheet() afecta a todas las ventanas abiertas (MainWindow e InfoUsuarioWindow).
 * El tema oscuro es el predeterminado; el claro se activa con el toggle.
 */
void MainWindow::aplicarTema()
{
    if (temaOscuro) {
        qApp->setStyleSheet(
            "QMainWindow, QDialog { background-color: #1e1e1e; color: #ffffff; }"
            "QWidget { background-color: #1e1e1e; color: #ffffff; }"
            "QPushButton { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555; padding: 5px; border-radius: 4px; }"
            "QPushButton:hover { background-color: #505050; }"
            "QPushButton:disabled { background-color: #2a2a2a; color: #666; }"
            "QLineEdit, QComboBox { background-color: #2d2d2d; color: #ffffff; border: 1px solid #555; padding: 4px; }"
            "QScrollArea { background-color: #1e1e1e; border: 1px solid #444; }"
            "QLabel { color: #ffffff; }"
            );
        ui->botonToggleTema->setText("☀ Tema Claro");
        ui->labelImagen->setStyleSheet("background-color: #2d2d2d; color: white;");
        ui->botonCerrarSesion->setStyleSheet("background-color: #c0392b; color: white;");
    } else {
        qApp->setStyleSheet(
            "QMainWindow, QDialog { background-color: #f0f0f0; color: #000000; }"
            "QWidget { background-color: #f0f0f0; color: #000000; }"
            "QPushButton { background-color: #dcdcdc; color: #000000; border: 1px solid #aaa; padding: 5px; border-radius: 4px; }"
            "QPushButton:hover { background-color: #c8c8c8; }"
            "QPushButton:disabled { background-color: #e8e8e8; color: #999; }"
            "QLineEdit, QComboBox { background-color: #ffffff; color: #000000; border: 1px solid #aaa; padding: 4px; }"
            "QScrollArea { background-color: #f0f0f0; border: 1px solid #ccc; }"
            "QLabel { color: #000000; }"
            );
        ui->botonToggleTema->setText("🌙 Tema Oscuro");
        ui->labelImagen->setStyleSheet("background-color: #d0d0d0; color: black;");
        ui->botonCerrarSesion->setStyleSheet("background-color: #c0392b; color: white;");
    }
}

// Slot: alterna el estado del tema y lo aplica
void MainWindow::onToggleTemaClicked()
{
    temaOscuro = !temaOscuro;
    aplicarTema();
}