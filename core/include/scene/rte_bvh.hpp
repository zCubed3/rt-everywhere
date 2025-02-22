#ifndef RTEVERYWHERE_RTE_BVH_HPP
#define RTEVERYWHERE_RTE_BVH_HPP

#include <vector>

#include <scene/rte_aabb.hpp>

struct rteBVHNode : public rteAABB {

private:
    int nodeID = -1; // The index of this node within the BVH

public:
    int GetNodeID() const {
        return nodeID;
    }

};

/// @brief A cached scene representation to allow faster scene lookups
class rteBVH {

protected:
    struct rteBVHBranch {
        int leftNodeID = -1, rightNodeID = -1;
    };

protected:
    std::vector<rteBVHNode> nodes;
    std::vector<rteBVHBranch> branches;

};

#endif //RTEVERYWHERE_RTE_BVH_HPP
