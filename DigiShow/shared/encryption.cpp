
#include "encryption.h"
#include "aes.hpp"
#include <QByteArray>
#include <QString>
#include <QCryptographicHash>

// Salt for IV derivation
#define IV_SALT "IV"

// PKCS7 Padding
QByteArray pkcs7Padding(const QByteArray &data, int blockSize) {
    int paddingSize = blockSize - (data.size() % blockSize);
    QByteArray padding(paddingSize, paddingSize);
    return data + padding;
}

QByteArray pkcs7Unpadding(const QByteArray &data) {
    if (data.isEmpty()) {
        return data;
    }
    int paddingSize = data.at(data.size() - 1);
    return data.left(data.size() - paddingSize);
}

QByteArray encryptString(const QString &plainText, const QString &key) {
    // Derive key and IV from the user-provided key
    QByteArray keyData = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256);
    QByteArray ivData = QCryptographicHash::hash(key.toUtf8().append(IV_SALT), QCryptographicHash::Sha256);
    ivData.truncate(16);

    AES_ctx ctx;
    AES_init_ctx_iv(&ctx, (uint8_t*)keyData.constData(), (uint8_t*)ivData.constData());

    QByteArray plainTextBytes = plainText.toUtf8();
    QByteArray paddedData = pkcs7Padding(plainTextBytes, AES_BLOCKLEN);

    AES_CBC_encrypt_buffer(&ctx, (uint8_t*)paddedData.data(), paddedData.size());

    return paddedData;
}

QString decryptString(const QByteArray &cipherText, const QString &key) {
    // Derive key and IV from the user-provided key
    QByteArray keyData = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256);
    QByteArray ivData = QCryptographicHash::hash(key.toUtf8().append(IV_SALT), QCryptographicHash::Sha256);
    ivData.truncate(16);

    AES_ctx ctx;
    AES_init_ctx_iv(&ctx, (uint8_t*)keyData.constData(), (uint8_t*)ivData.constData());

    QByteArray decryptedData = cipherText;
    AES_CBC_decrypt_buffer(&ctx, (uint8_t*)decryptedData.data(), decryptedData.size());

    QByteArray unpaddedData = pkcs7Unpadding(decryptedData);

    return QString::fromUtf8(unpaddedData);
}
