#include "folder.h"

Folder::Folder()
    : m_id(0)
    , m_color("#64748B")
    , m_position(0)
{
}

Folder::Folder(int id, const QString &name)
    : m_id(id)
    , m_name(name)
    , m_color("#64748B")
    , m_position(0)
{
}
