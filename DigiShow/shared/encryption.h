
#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QString>
#include <QByteArray>

// Encrypt a string
QByteArray encryptString(const QString &plainText, const QString &key);

// Decrypt a string
QString decryptString(const QByteArray &cipherText, const QString &key);

#endif // ENCRYPTION_H
