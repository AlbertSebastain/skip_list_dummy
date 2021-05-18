# ifndef LOCK_H
# define LOCK_H
# include <exception>
# include <semaphore.h>
using namespace std;
class sem
{
    public:
        sem();
        ~sem();
        bool wait();
        bool post();
    private:
        sem_t m_sem;
};
sem::sem()
{
    if(sem_init(&m_sem,0,0) != 0)
    {
        throw exception();
    }
}
sem::~sem()
{
    sem_destroy(&m_sem);
}
bool sem::wait()
{
    return sem_wait(&m_sem) == 0;
}
bool sem::post()
{
    return sem_post(&m_sem);
}
# endif