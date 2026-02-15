#include "tag.h"

Tag::Tag()
    : m_id(0)
    , m_color("#3B82F6")
{
}

Tag::Tag(int id, const QString &name, const QString &color)
    : m_id(id)
    , m_name(name)
    , m_color(color)
{
}
