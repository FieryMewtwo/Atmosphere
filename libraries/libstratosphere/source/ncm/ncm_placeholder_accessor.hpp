/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stratosphere.hpp>

namespace ams::ncm {

    class PlaceHolderAccessor {
        private:
            struct CacheEntry {
                PlaceHolderId id;
                fs::FileHandle handle;
                u64 counter;
            };

            static constexpr size_t MaxCacheEntries = 0x2;
        private:
            std::array<CacheEntry, MaxCacheEntries> m_caches;
            PathString *m_root_path;
            u64 m_cur_counter;
            os::SdkMutex m_cache_mutex;
            MakePlaceHolderPathFunction m_make_placeholder_path_func;
            bool m_delay_flush;
        private:
            Result Open(fs::FileHandle *out_handle, PlaceHolderId placeholder_id);
            bool LoadFromCache(fs::FileHandle *out_handle, PlaceHolderId placeholder_id);
            void StoreToCache(PlaceHolderId placeholder_id, fs::FileHandle handle);
            void Invalidate(CacheEntry *entry);
            CacheEntry *FindInCache(PlaceHolderId placeholder_id);
            CacheEntry *GetFreeEntry();;
        public:
            PlaceHolderAccessor() : m_cur_counter(0), m_cache_mutex(), m_delay_flush(false) {
                for (size_t i = 0; i < MaxCacheEntries; i++) {
                    m_caches[i].id = InvalidPlaceHolderId;
                }
            }

            ~PlaceHolderAccessor() { this->InvalidateAll(); }

            static void MakeBaseDirectoryPath(PathString *out, const char *root_path);
            static Result GetPlaceHolderIdFromFileName(PlaceHolderId *out, const char *name);
        public:
            /* API. */
            void Initialize(PathString *root, MakePlaceHolderPathFunction path_func, bool delay_flush) {
                m_root_path = root;
                m_make_placeholder_path_func = path_func;
                m_delay_flush = delay_flush;
            }

            Result CreatePlaceHolderFile(PlaceHolderId placeholder_id, s64 size);
            Result DeletePlaceHolderFile(PlaceHolderId placeholder_id);
            Result WritePlaceHolderFile(PlaceHolderId placeholder_id, s64 offset, const void *buffer, size_t size);
            Result SetPlaceHolderFileSize(PlaceHolderId placeholder_id, s64 size);
            Result TryGetPlaceHolderFileSize(bool *out_found, s64 *out_size, PlaceHolderId placeholder_id);

            void GetPath(PathString *out_placeholder_path, PlaceHolderId placeholder_id);
            void MakePath(PathString *out_placeholder_path, PlaceHolderId placeholder_id) const;

            void InvalidateAll();

            Result EnsurePlaceHolderDirectory(PlaceHolderId placeholder_id);
            size_t GetHierarchicalDirectoryDepth() const { return GetHierarchicalPlaceHolderDirectoryDepth(m_make_placeholder_path_func); }
    };

}
