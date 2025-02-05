/*
 *  Copyright (c) 2020 NetEase Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * Project: curve
 * File Created: Friday, 21st June 2019 3:09:09 pm
 * Author: tongguangxun
 */

#ifndef SRC_CLIENT_CLIENT_METRIC_H_
#define SRC_CLIENT_CLIENT_METRIC_H_

#include <bvar/bvar.h>

#include <string>
#include <vector>

#include "src/common/timeutility.h"
#include "src/client/client_common.h"
#include "src/common/string_util.h"

using curve::common::TimeUtility;

namespace curve {
namespace client {

inline void GetStringValue(std::ostream& os, void* arg) {
    os << *static_cast<std::string*>(arg);
}

struct SlowRequestMetric {
    bvar::Adder<uint64_t> count;

    SlowRequestMetric(const std::string& prefix, const std::string& name)
        : count(prefix, name + "_total") {}
};

// 秒级信息统计
struct PerSecondMetric {
    // 当前persecond计数总数
    bvar::Adder<uint64_t> count;
    // persecond真实数据，这个数据依赖于count
    bvar::PerSecond<bvar::Adder<uint64_t>> value;

    PerSecondMetric(const std::string& prefix, const std::string& name)
        : count(prefix, name + "_total_count"),
          value(prefix, name, &count, 1) {}
};

// 接口统计信息metric信息统计
struct InterfaceMetric {
    // 接口统计信息调用qps
    PerSecondMetric qps;
    // error request persecond
    PerSecondMetric eps;
    // receive request persecond
    PerSecondMetric rps;
    // 调用吞吐
    PerSecondMetric bps;
    // 调用超时次数qps
    PerSecondMetric timeoutQps;
    // 调用redirect次数qps
    PerSecondMetric redirectQps;
    // 调用latency
    bvar::LatencyRecorder latency;

    InterfaceMetric(const std::string& prefix, const std::string& name)
        : qps(prefix, name + "_qps"),
          eps(prefix, name + "_eps"),
          rps(prefix, name + "_rps"),
          bps(prefix, name + "_bps"),
          timeoutQps(prefix, name + "_timeout_qps"),
          redirectQps(prefix, name + "_redirect_qps"),
          latency(prefix, name + "_lat") {}
};

struct DiscardMetric {
    explicit DiscardMetric(const std::string& prefix)
        : totalSuccess(prefix, "discard_total_success"),
          totalError(prefix, "discard_total_error"),
          totalCanceled(prefix, "discard_total_canceled"),
          pending(prefix, "discard_pending") {}

    bvar::Adder<int64_t> totalSuccess;
    bvar::Adder<int64_t> totalError;
    bvar::Adder<int64_t> totalCanceled;
    bvar::Adder<int64_t> pending;
};

// 文件级别metric信息统计
struct FileMetric {
    const std::string prefix = "curve_client";

    // 当前metric归属于哪个文件
    std::string filename;

    // 当前文件inflight io数量
    bvar::Adder<int64_t> inflightRPCNum;

    // 当前文件请求的最大请求字节数，这种统计方式可以很方便的看到最大值，分位值
    bvar::LatencyRecorder readSizeRecorder;
    bvar::LatencyRecorder writeSizeRecorder;
    bvar::LatencyRecorder discardSizeRecorder;

    // libcurve最底层read rpc接口统计信息metric统计
    InterfaceMetric readRPC;
    // libcurve最底层write rpc接口统计信息metric统计
    InterfaceMetric writeRPC;
    // 用户读请求qps、eps、rps
    InterfaceMetric userRead;
    // 用户写请求qps、eps、rps
    InterfaceMetric userWrite;
    // user's discard request
    InterfaceMetric userDiscard;

    // get leader失败重试qps
    PerSecondMetric getLeaderRetryQPS;

    // Number of slow requests
    SlowRequestMetric slowRequestMetric;

    DiscardMetric discardMetric;

    explicit FileMetric(const std::string& name)
        : filename(name),
          inflightRPCNum(prefix, filename + "_inflight_rpc_num"),
          readSizeRecorder(prefix, filename + "_read_request_size_recoder"),
          writeSizeRecorder(prefix, filename + "_write_request_size_recoder"),
          discardSizeRecorder(prefix,
                              filename + "_discard_request_size_recoder"),
          readRPC(prefix, filename + "_read_rpc"),
          writeRPC(prefix, filename + "_write_rpc"),
          userRead(prefix, filename + "_read"),
          userWrite(prefix, filename + "_write"),
          userDiscard(prefix, filename + "_discard"),
          getLeaderRetryQPS(prefix, filename + "_get_leader_retry_rpc"),
          slowRequestMetric(prefix, filename + "_slow_request"),
          discardMetric(prefix + filename) {}
};

// 用于全局mds接口统计信息调用信息统计
struct MDSClientMetric {
    std::string prefix;

    // mds的地址信息
    std::string metaserverAddr;
    bvar::PassiveStatus<std::string> metaserverAddress;

    // openfile接口统计信息
    InterfaceMetric openFile;
    // createFile接口统计信息
    InterfaceMetric createFile;
    // closeFile接口统计信息
    InterfaceMetric closeFile;
    // getFileInfo接口统计信息
    InterfaceMetric getFile;
    // RefreshSession接口统计信息
    InterfaceMetric refreshSession;
    // GetServerList接口统计信息
    InterfaceMetric getServerList;
    // GetOrAllocateSegment接口统计信息
    InterfaceMetric getOrAllocateSegment;
    // DeAllocateSegment接口统计信息
    InterfaceMetric deAllocateSegment;
    // RenameFile接口统计信息
    InterfaceMetric renameFile;
    // Extend接口统计信息
    InterfaceMetric extendFile;
    // DeleteFile接口统计信息
    InterfaceMetric deleteFile;
    // RecoverFile interface metric
    InterfaceMetric recoverFile;
    // changeowner接口统计信息
    InterfaceMetric changeOwner;
    // listdir接口统计信息
    InterfaceMetric listDir;
    // register接口统计信息
    InterfaceMetric registerClient;
    // GetChunkServerID接口统计
    InterfaceMetric getChunkServerId;
    // ListChunkServerInServer接口统计
    InterfaceMetric listChunkserverInServer;
    // IncreaseEpoch
    InterfaceMetric increaseEpoch;

    // 切换mds server总次数
    bvar::Adder<uint64_t> mdsServerChangeTimes;

    explicit MDSClientMetric(const std::string& prefix_ = "")
        : prefix(!prefix_.empty()
                     ? prefix_
                     : "curve_mds_client_" + common::ToHexString(this)),
          metaserverAddress(prefix, "current_metaserver_addr", GetStringValue,
                            &metaserverAddr),
          openFile(prefix, "openFile"),
          createFile(prefix, "createFile"),
          closeFile(prefix, "closeFile"),
          getFile(prefix, "getFileInfo"),
          refreshSession(prefix, "refreshSession"),
          getServerList(prefix, "getServerList"),
          getOrAllocateSegment(prefix, "getOrAllocateSegment"),
          deAllocateSegment(prefix, "deAllocateSegment"),
          renameFile(prefix, "renameFile"),
          extendFile(prefix, "extendFile"),
          deleteFile(prefix, "deleteFile"),
          recoverFile(prefix, "recoverFile"),
          changeOwner(prefix, "changeOwner"),
          listDir(prefix, "listDir"),
          registerClient(prefix, "registerClient"),
          getChunkServerId(prefix, "GetChunkServerId"),
          listChunkserverInServer(prefix, "ListChunkServerInServer"),
          increaseEpoch(prefix, "IncreaseEpoch"),
          mdsServerChangeTimes(prefix, "mds_server_change_times") {}
};

struct LatencyGuard {
    bvar::LatencyRecorder* latencyRec;
    uint64_t startTimeUs;

    explicit LatencyGuard(bvar::LatencyRecorder* latency) {
        latencyRec = latency;
        startTimeUs = TimeUtility::GetTimeofDayUs();
    }

    ~LatencyGuard() {
        *latencyRec << (TimeUtility::GetTimeofDayUs() - startTimeUs);
    }
};

class MetricHelper {
 public:
    /**
     * 统计getleader重试次数
     * @param: fm为当前文件的metric指针
     */
    static void IncremGetLeaderRetryTime(FileMetric* fm) {
        if (fm != nullptr) {
            fm->getLeaderRetryQPS.count << 1;
        }
    }

    /**
     * 统计用户当前读写请求次数，用于qps计算
     * @param: fm为当前文件的metric指针
     * @param: length为当前请求大小
     * @param: read为当前操作是读操作还是写操作
     */
    static void IncremUserQPSCount(FileMetric* fm,
                                   uint64_t length,
                                   OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->userRead.qps.count << 1;
                    fm->userRead.bps.count << length;
                    fm->readSizeRecorder << length;
                    break;
                case OpType::WRITE:
                    fm->userWrite.qps.count << 1;
                    fm->userWrite.bps.count << length;
                    fm->writeSizeRecorder << length;
                    break;
                case OpType::DISCARD:
                    fm->userDiscard.qps.count << 1;
                    fm->userDiscard.bps.count << length;
                    fm->discardSizeRecorder << length;
                default:
                    break;
            }
        }
    }

    /**
     * 统计用户当前读写请求失败次数，用于eps计算
     * @param: fm为当前文件的metric指针
     * @param: read为当前操作是读操作还是写操作
     */
    static void IncremUserEPSCount(FileMetric* fm, OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->userRead.eps.count << 1;
                    break;
                case OpType::WRITE:
                    fm->userWrite.eps.count << 1;
                    break;
                case OpType::DISCARD:
                    fm->userDiscard.eps.count << 1;
                default:
                    break;
            }
        }
    }

    /**
     * 统计用户当前接收到的读写请求次数，用于rps计算
     * rps: receive request persecond, 就是当前接口每秒接收到的请求数量
     * qps: query request persecond, 就是当前接口每秒处理的请求数量
     * eps: error request persecond, 就是当前接口每秒出错的请求数量
     * rps减去qps就是当前client端每秒钟等待的请求数量，这部分请求会持久占用当前一秒内的内存
     * @param: fm为当前文件的metric指针
     * @param: read为当前操作是读操作还是写操作
     */
    static void IncremUserRPSCount(FileMetric* fm, OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->userRead.rps.count << 1;
                    break;
                case OpType::WRITE:
                    fm->userWrite.rps.count << 1;
                    break;
                case OpType::DISCARD:
                    fm->userDiscard.rps.count << 1;
                default:
                    break;
            }
        }
    }

    /**
     * 统计当前rpc失败次数，用于eps计算
     * @param: fm为当前文件的metric指针
     * @param: read为当前操作是读操作还是写操作
     */
    static void IncremFailRPCCount(FileMetric* fm, OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->readRPC.eps.count << 1;
                    break;
                case OpType::WRITE:
                    fm->writeRPC.eps.count << 1;
                    break;
                default:
                    break;
            }
        }
    }

    /**
     * 统计用户当前读写请求超时次数，用于timeoutQps计算
     * @param: fm为当前文件的metric指针
     * @param: read为当前操作是读操作还是写操作
     */
    static void IncremTimeOutRPCCount(FileMetric* fm, OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->readRPC.timeoutQps.count << 1;
                    break;
                case OpType::WRITE:
                    fm->writeRPC.timeoutQps.count << 1;
                    break;
                default:
                    break;
            }
        }
    }

    /**
     * 统计请求被redirect的次数
     * @param fileMetric 当前文件的metric指针
     * @param opType 请求类型
     */
    static void IncremRedirectRPCCount(FileMetric* fileMetric, OpType opType) {
        if (fileMetric) {
            switch (opType) {
                case OpType::READ:
                    fileMetric->readRPC.redirectQps.count << 1;
                    break;
                case OpType::WRITE:
                    fileMetric->writeRPC.redirectQps.count << 1;
                    break;
                default:
                    break;
            }
        }
    }

    /**
     * 统计读写RPC接口统计信息请求次数及带宽统计，用于qps及bps计算
     * @param: fm为当前文件的metric指针
     * @param: length为当前请求大小
     * @param: read为当前操作是读操作还是写操作
     */
    static void IncremRPCQPSCount(FileMetric* fm,
                                  uint64_t length,
                                  OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->readRPC.qps.count << 1;
                    fm->readRPC.bps.count << length;
                    break;
                case OpType::WRITE:
                    fm->writeRPC.qps.count << 1;
                    fm->writeRPC.bps.count << length;
                    break;
                default:
                    break;
            }
        }
    }

    /**
     * 统计读写RPC接口统计信息请求次数及带宽统计，用于rps计算
     * @param: fm为当前文件的metric指针
     * @param: length为当前请求大小
     * @param: read为当前操作是读操作还是写操作
     */
    static void IncremRPCRPSCount(FileMetric* fm,
                                  OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->readRPC.rps.count << 1;
                    break;
                case OpType::WRITE:
                    fm->writeRPC.rps.count << 1;
                    break;
                default:
                    break;
            }
        }
    }

    static void LatencyRecord(FileMetric* fm,
                              uint64_t duration,
                              OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->readRPC.latency << duration;
                    break;
                case OpType::WRITE:
                    fm->writeRPC.latency << duration;
                    break;
                default:
                    break;
            }
        }
    }

    static void UserLatencyRecord(FileMetric* fm,
                                  uint64_t duration,
                                  OpType type) {
        if (fm != nullptr) {
            switch (type) {
                case OpType::READ:
                    fm->userRead.latency << duration;
                    break;
                case OpType::WRITE:
                    fm->userWrite.latency << duration;
                    break;
                case OpType::DISCARD:
                    fm->userDiscard.latency << duration;
                default:
                    break;
            }
        }
    }

    static void IncremInflightRPC(FileMetric* fm) {
        if (fm != nullptr) {
            fm->inflightRPCNum << 1;
        }
    }

    static void DecremInflightRPC(FileMetric* fm) {
        if (fm != nullptr) {
            fm->inflightRPCNum << -1;
        }
    }

    static void IncremSlowRequestNum(FileMetric* fm) {
        if (fm != nullptr) {
            fm->slowRequestMetric.count << 1;
        }
    }

    static void DecremSlowRequestNum(FileMetric* fm) {
        if (fm != nullptr) {
            fm->slowRequestMetric.count.get_value() > 0
                ? fm->slowRequestMetric.count << -1
                : fm->slowRequestMetric.count << 0;
        }
    }
};
}   // namespace client
}   // namespace curve

#endif  // SRC_CLIENT_CLIENT_METRIC_H_
