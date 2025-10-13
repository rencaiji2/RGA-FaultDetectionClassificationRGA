#ifndef SNUMBERVALIDATOR_H
#define SNUMBERVALIDATOR_H
#include <QLineEdit>
#include <QRegExpValidator>
#include <QDebug>

class sNumberValidator: public QValidator {
public:
    sNumberValidator(double min, double max, QObject* parent = nullptr)
        : QValidator(parent), m_min(min), m_max(max) {}
    QValidator::State validate(QString& input, int& pos) const override {
        Q_UNUSED(pos)
        if (input.isEmpty())
            return QValidator::Intermediate;

        bool ok;
        double value = input.toDouble(&ok);
        if (!ok)
            return QValidator::Invalid;

        if (value >= m_min && value <= m_max) {
            return QValidator::Acceptable;
        } else {
            return QValidator::Intermediate;
        }
    }
    static void setNumberValidator(QLineEdit* lineEdit, double min, double max){
        connect(lineEdit, &QLineEdit::textChanged, [=](const QString &text) {
            bool isValid = false;
            if (!text.isEmpty()) {
                double value = text.toDouble(&isValid);
                isValid = isValid && (value >= min && value <= max);
            }
            if (isValid || text.isEmpty()) {
                lineEdit->setStyleSheet("");
            } else {
                lineEdit->setStyleSheet("QLineEdit { background-color: #FFC0CB; }");
            }
        });
    }
private:
    double m_min, m_max;
};

#endif // SNUMBERVALIDATOR_H
