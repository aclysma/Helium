//----------------------------------------------------------------------------------------------------------------------
// JobManager.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_JOB_MANAGER_H
#define LUNAR_ENGINE_JOB_MANAGER_H

#include "Engine/JobPool.h"

#include "Platform/Thread.h"

#ifndef L_TRACK_JOB_POOL_HITS
/// Set to non-zero to track stats on which job allocations pull from an already pooled job object and which require a
/// new job allocation.
#define L_TRACK_JOB_POOL_HITS ( L_DEBUG )
#endif

namespace Lunar
{
    /// Job manager.
    class LUNAR_ENGINE_API JobManager : NonCopyable
    {
    public:
        /// @name Initialization
        //@{
        bool Initialize();
        void Shutdown();
        //@}

        /// @name Job Allocation
        //@{
        template< typename T > T* AllocateJob();
        template< typename T > void ReleaseJob( T* pJob );

        void* AllocateJobUninitialized( size_t size );
        void ReleaseJobUninitialized( void* pJob, size_t size );
        //@}

        /// @name Static Access
        //@{
        static JobManager& GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    private:
        /// Thread-local job pool list node.
        struct PoolNode
        {
            /// Job pool.
            JobPool pool;
            /// Next pool in the list.
            PoolNode* volatile pNext;
#if L_TRACK_JOB_POOL_HITS
            /// Hits on jobs pulled from the local thread's pool.
            uint32_t localHits;
            /// Hits on jobs pulled from other threads' pools.
            uint32_t stolenHits;
            /// Misses (new jobs needed to be allocated).
            uint32_t misses;
#endif
        };

        /// List of job pools for each thread.
        PoolNode* volatile m_pHeadPool;
        /// Thread-local storage for job pool data.
        ThreadLocalPointer m_poolTls;

        /// Job manager instance.
        static JobManager* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        JobManager();
        ~JobManager();
        //@}

        /// @name Private Utility Functions
        //@{
        PoolNode* GetThreadLocalPoolNode();
        //@}
    };
}

#include "Engine/JobManager.inl"

#endif  // LUNAR_ENGINE_JOB_MANAGER_H
