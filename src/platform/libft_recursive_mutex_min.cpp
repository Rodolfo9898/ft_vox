#include "../../Libft/Modules/CPP_class/class_nullptr.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/PThread/recursive_mutex.hpp"
#include <new>

static pt_thread_id_type ft_vox_empty_thread_id()
{
    return (pt_thread_id_type());
}

pt_recursive_mutex::pt_recursive_mutex()
    : _owner(ft_vox_empty_thread_id()), _lock(false), _lock_depth(0),
      _native_mutex(ft_nullptr),
      _initialised_state(pt_recursive_mutex::_state_uninitialised),
      _state_mutex(ft_nullptr), _valid_state(false)
{
    return ;
}

pt_recursive_mutex::~pt_recursive_mutex()
{
    (void)this->destroy();
    return ;
}

int pt_recursive_mutex::ensure_native_mutex() const
{
    if (this->_initialised_state != pt_recursive_mutex::_state_initialised
        || this->_native_mutex == ft_nullptr)
        return (FT_ERR_INVALID_STATE);
    return (FT_ERR_SUCCESS);
}

int pt_recursive_mutex::initialize()
{
    if (this->_initialised_state == pt_recursive_mutex::_state_initialised)
        return (FT_ERR_INVALID_STATE);
    this->_native_mutex = new (std::nothrow) std::mutex();
    if (this->_native_mutex == ft_nullptr)
    {
        this->_initialised_state = pt_recursive_mutex::_state_destroyed;
        this->_valid_state.store(false, std::memory_order_release);
        return (FT_ERR_NO_MEMORY);
    }
    this->_owner.store(ft_vox_empty_thread_id(), std::memory_order_release);
    this->_lock.store(false, std::memory_order_release);
    this->_lock_depth.store(0, std::memory_order_release);
    this->_initialised_state = pt_recursive_mutex::_state_initialised;
    this->_valid_state.store(true, std::memory_order_release);
    return (FT_ERR_SUCCESS);
}

int pt_recursive_mutex::destroy()
{
    if (this->_native_mutex != ft_nullptr)
    {
        if (this->_lock.load(std::memory_order_acquire) == true)
            return (FT_ERR_THREAD_BUSY);
        delete this->_native_mutex;
        this->_native_mutex = ft_nullptr;
    }
    this->_owner.store(ft_vox_empty_thread_id(), std::memory_order_release);
    this->_lock.store(false, std::memory_order_release);
    this->_lock_depth.store(0, std::memory_order_release);
    this->_initialised_state = pt_recursive_mutex::_state_destroyed;
    this->_valid_state.store(false, std::memory_order_release);
    return (FT_ERR_SUCCESS);
}

bool pt_recursive_mutex::lockState() const
{
    return (this->_lock.load(std::memory_order_acquire));
}

int pt_recursive_mutex::lock_internal(bool *lock_acquired) const
{
    if (lock_acquired != ft_nullptr)
        *lock_acquired = false;
    return (FT_ERR_SUCCESS);
}

int pt_recursive_mutex::unlock_internal(bool lock_acquired) const
{
    (void)lock_acquired;
    return (FT_ERR_SUCCESS);
}

void pt_recursive_mutex::teardown_thread_safety()
{
    return ;
}

int pt_recursive_mutex::lock_state(bool *lock_acquired) const
{
    return (this->lock_internal(lock_acquired));
}

void pt_recursive_mutex::unlock_state(bool lock_acquired) const
{
    (void)this->unlock_internal(lock_acquired);
    return ;
}

bool pt_recursive_mutex::is_owned_by_thread(pt_thread_id_type thread_id) const
{
    if (this->_lock.load(std::memory_order_acquire) == false)
        return (false);
    return (pthread_equal(this->_owner.load(std::memory_order_relaxed),
        thread_id) != 0);
}

int pt_recursive_mutex::lock() const
{
    pt_thread_id_type thread_id;

    if (this->ensure_native_mutex() != FT_ERR_SUCCESS)
        return (FT_ERR_INVALID_STATE);
    thread_id = pthread_self();
    if (this->_lock.load(std::memory_order_acquire) == true
        && pthread_equal(this->_owner.load(std::memory_order_relaxed),
            thread_id) != 0)
    {
        this->_lock_depth.fetch_add(1, std::memory_order_relaxed);
        return (FT_ERR_SUCCESS);
    }
    this->_native_mutex->lock();
    this->_owner.store(thread_id, std::memory_order_release);
    this->_lock.store(true, std::memory_order_release);
    this->_lock_depth.store(1, std::memory_order_release);
    return (FT_ERR_SUCCESS);
}

int pt_recursive_mutex::unlock() const
{
    ft_size_t lock_depth;

    if (this->ensure_native_mutex() != FT_ERR_SUCCESS)
        return (FT_ERR_INVALID_STATE);
    if (this->_lock.load(std::memory_order_acquire) == false
        || pthread_equal(this->_owner.load(std::memory_order_relaxed),
            pthread_self()) == 0)
        return (FT_ERR_INVALID_STATE);
    lock_depth = this->_lock_depth.load(std::memory_order_relaxed);
    if (lock_depth > 1)
    {
        this->_lock_depth.store(lock_depth - 1, std::memory_order_release);
        return (FT_ERR_SUCCESS);
    }
    this->_owner.store(ft_vox_empty_thread_id(), std::memory_order_release);
    this->_lock_depth.store(0, std::memory_order_release);
    this->_lock.store(false, std::memory_order_release);
    this->_native_mutex->unlock();
    return (FT_ERR_SUCCESS);
}

int pt_recursive_mutex::try_lock(pt_thread_id_type thread_id) const
{
    if (this->ensure_native_mutex() != FT_ERR_SUCCESS)
        return (FT_ERR_INVALID_STATE);
    if (this->_lock.load(std::memory_order_acquire) == true
        && pthread_equal(this->_owner.load(std::memory_order_relaxed),
            thread_id) != 0)
    {
        this->_lock_depth.fetch_add(1, std::memory_order_relaxed);
        return (FT_ERR_SUCCESS);
    }
    if (this->_native_mutex->try_lock() == false)
        return (FT_ERR_THREAD_BUSY);
    this->_owner.store(thread_id, std::memory_order_release);
    this->_lock.store(true, std::memory_order_release);
    this->_lock_depth.store(1, std::memory_order_release);
    return (FT_ERR_SUCCESS);
}
