#ifndef TAG_H
#define TAG_H

#include <QString>

class Tag
{
public:
    Tag();
    Tag(int id, const QString &name, const QString &color);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString color() const { return m_color; }
    void setColor(const QString &color) { m_color = color; }

private:
    int m_id;
    QString m_name;
    QString m_color;
};

#endif // TAG_H
