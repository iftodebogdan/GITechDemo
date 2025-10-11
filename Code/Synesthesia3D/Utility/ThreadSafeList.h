#ifndef THREADSAFELIST_H_
#define THREADSAFELIST_H_

#include <shared_mutex>

namespace Synesthesia3D
{
    template <typename T>
    class ThreadSafeList
    {
    public:
        class Reader
        {
            Reader(ThreadSafeList& arrList) : m_pList(&arrList) { m_pList->m_tListLock.lock_shared(); }

        public:
            ~Reader() { m_pList->m_tListLock.unlock_shared(); }

            const std::vector<T>& GetList() const { return m_pList->m_arrList; }

            ThreadSafeList* m_pList;

            friend class ThreadSafeList;
        };

        class Writer
        {
            Writer(ThreadSafeList& arrList) : m_pList(&arrList) { arrList.m_tListLock.lock(); }

        public:
            ~Writer() { m_pList->m_tListLock.unlock(); }

            std::vector<T>& GetList() { return m_pList->m_arrList; }

            ThreadSafeList* m_pList;

            friend class ThreadSafeList;
        };

        Reader GetReader() const { return Reader(*const_cast<ThreadSafeList*>(this)); }
        Writer GetWriter() { return Writer(*this); }

        //std::vector<T>& GetListUnsafe() { return m_arrList; }
        //const std::vector<T>& GetListUnsafe() const { return m_arrList; }

    private:
        std::vector<T> m_arrList;
        std::shared_mutex m_tListLock;
    };
}

#endif // THREADSAFELIST_H_