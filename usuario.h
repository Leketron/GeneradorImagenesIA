#ifndef USUARIO_H
#define USUARIO_H

#include <QString>

/*
 * Clase base: Usuario
 * Define la estructura común para todos los tipos de usuario del sistema.
 * Utiliza funciones virtuales para permitir polimorfismo:
 * cada clase derivada puede redefinir el comportamiento según su rol.
 */
class Usuario {
public:

    // Constructor: inicializa nombre, contraseña y el contador de imágenes en 0
    Usuario(QString nombre, QString contrasena)
        : nombre(nombre), contrasena(contrasena), imagenesGeneradas(0) {}

    // Getter del nombre de usuario (solo lectura)
    QString getNombre() const { return nombre; }

    // Verifica si la contraseña ingresada coincide con la almacenada
    bool verificarContrasena(const QString &pass) const {
        return contrasena == pass;
    }

    // Incrementa en 1 el contador de imágenes generadas en la sesión
    void incrementarContador() { imagenesGeneradas++; }

    // Retorna la cantidad de imágenes generadas en la sesión actual
    int getImagenesGeneradas() const { return imagenesGeneradas; }

    /*
     * Funciones virtuales: permiten que cada clase derivada defina
     * su propio comportamiento según el tipo de usuario (polimorfismo).
     * La clase base define valores por defecto para un usuario sin privilegios.
     */

    // Retorna true si el usuario es administrador (por defecto: no)
    virtual bool esAdmin() const { return false; }

    // Retorna una cadena con el tipo de usuario (por defecto: "Usuario Común")
    virtual QString getTipo() const { return "Usuario Común"; }

    // Indica si el usuario puede seguir generando imágenes (por defecto: siempre sí)
    virtual bool puedeGenerar() const { return true; }

    // Retorna el límite de imágenes por sesión (-1 indica ilimitado)
    virtual int getLimite() const { return -1; }

    // Destructor virtual: necesario para liberar memoria correctamente
    // cuando se elimina un objeto a través de un puntero a la clase base
    virtual ~Usuario() {}

protected:
    // Atributos encapsulados: accesibles por las clases derivadas pero no desde afuera
    QString nombre;
    QString contrasena;
    int imagenesGeneradas; // Contador de imágenes generadas en la sesión
};

/*
 * Clase derivada: UsuarioComun
 * Hereda de Usuario y representa un usuario estándar del sistema.
 * Restringe la generación de imágenes a un máximo de 5 por sesión.
 */
class UsuarioComun : public Usuario {
public:

    // Constructor: delega la inicialización a la clase base
    UsuarioComun(QString nombre, QString contrasena)
        : Usuario(nombre, contrasena) {}

    // Override: un usuario común no es administrador
    bool esAdmin() const override { return false; }

    // Override: identifica el tipo como "Usuario Común"
    QString getTipo() const override { return "Usuario Común"; }

    // Override: permite generar imágenes solo si no alcanzó el límite de 5
    bool puedeGenerar() const override { return imagenesGeneradas < 5; }

    // Override: el límite para este tipo de usuario es 5 imágenes por sesión
    int getLimite() const override { return 5; }
};

/*
 * Clase derivada: Administrador
 * Hereda de Usuario y representa un usuario con privilegios extendidos.
 * No tiene límite de imágenes y puede acceder a funciones exclusivas
 * como el historial y el cambio de tema visual.
 */
class Administrador : public Usuario {
public:

    // Constructor: delega la inicialización a la clase base
    Administrador(QString nombre, QString contrasena)
        : Usuario(nombre, contrasena) {}

    // Override: el administrador sí tiene privilegios de admin
    bool esAdmin() const override { return true; }

    // Override: identifica el tipo como "Administrador"
    QString getTipo() const override { return "Administrador"; }

    // Override: el administrador puede generar imágenes sin restricciones
    bool puedeGenerar() const override { return true; }

    // Override: sin límite de imágenes (-1 = ilimitado)
    int getLimite() const override { return -1; }
};

#endif // USUARIO_H