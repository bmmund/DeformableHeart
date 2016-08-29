#include "acm.hpp"
#include <algorithm>
#include <assert.h>
ACM::ACM()
{

}

void ACM::add(const CMap &cm)
{
    cm_list.push_back(cm);
}

std::vector<CMap>& ACM::connectivityMaps()
{
    return cm_list;
}

void ACM::clear()
{
    cm_list.clear();
    v_list.clear();
}

CMapHandle ACM::addCmap()
{
    CMap cm;
    cm.idx = cm_list.size();

    // allocate space
    std::vector<VertexHandle> v(2, -1);
    cm.cm.resize(2, v);

    cm_list.push_back(cm);
    return cm_list.back().idx;
}

VertexHandle ACM::addVertex()
{
    Vertex vert;
    return addVertex(vert);
}

VertexHandle ACM::addVertex(Vertex new_vertex)
{
    new_vertex.idx = v_list.size();
    v_list.push_back(new_vertex);
    return v_list.back().idx;
}

Vertex* ACM::getVertex(VertexHandle idx)
{
    return &v_list.at(idx);
}

CMap* ACM::getCMap(CMapHandle idx)
{
    return &cm_list.at(idx);
}

void ACM::addVertsToCMap(CMapHandle cm_idx,
                         VertexHandle vh00,
                         VertexHandle vh01,
                         VertexHandle vh10,
                         VertexHandle vh11)
{
    CMap& cm(cm_list.at(cm_idx));
    cm.cm[0][0] = vh00;
    cm.cm[0][1] = vh01;
    cm.cm[1][0] = vh10;
    cm.cm[1][1] = vh11;
    // update v_cm_map
    std::vector<VertexHandle> vhs = {vh00, vh01, vh10, vh11};
    updateVertexCMapMap(vhs, cm.idx);
}

void ACM::setFaceColour(CMapHandle cm_idx, glm::vec3 colour)
{
    cm_list.at(cm_idx).colour = colour;
}

std::vector<std::array<VertexHandle, 3>> ACM::getCMapFaces(CMapHandle cm_idx)
{
    std::vector<std::array<VertexHandle, 3>> faces;
    CMap* cmap = getCMap(cm_idx);
    for(int i = 0; i < cmap->cm.size()-1; i+=cmap->vectorScale)
    {
        for(int j = 0; j < cmap->cm.at(i).size()-1; j+=cmap->vectorScale)
        {
            std::array<VertexHandle, 3> verts;
            // left triangle
            verts[0] = cmap->cm.at(i).at(j);
            verts[1] = cmap->cm.at(i+cmap->vectorScale).at(j);
            verts[2] = cmap->cm.at(i).at(j+cmap->vectorScale);
            faces.push_back(verts);
            // right triangle
            verts[0] = cmap->cm.at(i).at(j+cmap->vectorScale);
            verts[1] = cmap->cm.at(i+cmap->vectorScale).at(j);
            verts[2] = cmap->cm.at(i+cmap->vectorScale).at(j+cmap->vectorScale);
            faces.push_back(verts);
        }
    }
    return faces;
}

void ACM::updateCMapNeighbours()
{
    for(auto& cm : cm_list)
    {
        VertexHandle vh00, vh01, vh10, vh11;
        CMapNeighbour cm_neighbour;
        std::vector<CMapHandle> cmhs;
        vh00 = cm.cm[0][0];
        vh01 = cm.cm[0][1];
        vh10 = cm.cm[1][0];
        vh11 = cm.cm[1][1];

        // edge vh00 vh10 - bottom edge
        cm_neighbour = getCommanCMap(cm.idx, glm::uvec2(0,0), glm::uvec2(1,0));
        assert(cm_neighbour.n == 0);
        cm.boundaryCMs[cm_neighbour.n] = cm_neighbour;

        // edge vh10 vh11 - right edge
        cm_neighbour = getCommanCMap(cm.idx, glm::uvec2(1,0), glm::uvec2(1,1));
        assert(cm_neighbour.n == 1);
        cm.boundaryCMs[cm_neighbour.n] = cm_neighbour;

        // edge vh01 vh11 - top edge
        cm_neighbour = getCommanCMap(cm.idx, glm::uvec2(0,1), glm::uvec2(1,1));
        assert(cm_neighbour.n == 2);
        cm.boundaryCMs[cm_neighbour.n] = cm_neighbour;

        // edge vh00 vh01 - left edge
        cm_neighbour = getCommanCMap(cm.idx, glm::uvec2(0,0), glm::uvec2(0,1));
        assert(cm_neighbour.n == 3);
        cm.boundaryCMs[cm_neighbour.n] = cm_neighbour;
    }
}


CMapHandle ACM::getCMapForVertex(VertexHandle vh)
{
    CMapHandle cm_idx = -1;
    if(v_cm_map.find(vh) != v_cm_map.end()) // found
    {
        cm_idx = v_cm_map[vh];
    }
    return cm_idx;
}

void ACM::refine()
{
    for(auto& cm : cm_list)
    {
        if(cm.vectorScale > 1)
        {
            cm.vectorScale = std::max((cm.vectorScale / 2), 1);
            continue;
        }

        // Create new temp container for filling
        int origSize = cm.cm.size();
        int newSize = (2*origSize) - 1;
        std::vector<VertexHandle> v(newSize, -1);
        std::vector<std::vector<VertexHandle>> cm_temp(newSize, v);

        // put elements into their correct position
        for(int i = 0; i < cm.cm.size(); i++)
        {
            for(int j = 0; j < cm.cm.at(i).size(); j++)
            {
                cm_temp.at(2*i).at(2*j) = cm.cm.at(i).at(j);
            }
        }

        // replace cm with new bigger cm
        cm.cm = cm_temp;

        for(int i = 0; i < cm.cm.size(); i++)
        {
            for(int j = 0; j < cm.cm.size(); j++)
            {
                if(cm.cm.at(i).at(j) == -1)
                {
                    VertexHandle vert = addVertex();
                    cm.cm.at(i).at(j) = vert;
                    updateVertexCMapMap(vert, cm.idx);
                }
            }
        }

        // fill new verts with average values
        for(int i = 0; i < cm.cm.size(); i+=2)
        {
            for(int j = 1; j < cm.cm.at(i).size(); j+=2)
            {
                Vertex* v1 = getVertex(cm.cm.at(i).at(j-1));
                Vertex* v2 = getVertex(cm.cm.at(i).at(j+1));
                Vertex* vnew = getVertex(cm.cm.at(i).at(j));
                vnew->point = (v1->point + v2->point) / 2.0f;
                vnew->colour = (v1->colour + v2->colour) / 2.0f;
                vnew->normal = (v1->normal + v2->normal) / 2.0f;
            }
        }

        for(int i = 1; i < cm.cm.size(); i+=2)
        {
            for(int j = 0; j < cm.cm.size(); j++)
            {
                Vertex* v1 = getVertex(cm.cm.at(i-1).at(j));
                Vertex* v2 = getVertex(cm.cm.at(i+1).at(j));
                Vertex* vnew = getVertex(cm.cm.at(i).at(j));
                vnew->point = (v1->point + v2->point) / 2.0f;
                vnew->colour = (v1->colour + v2->colour) / 2.0f;
                vnew->normal = (v1->normal + v2->normal) / 2.0f;
            }
        }
    }
}

void ACM::decompose()
{
    for(auto& cm : cm_list)
    {
        int maxVectorScale = cm.cm.size()-1;
        cm.vectorScale = std::min((cm.vectorScale * 2), maxVectorScale);
    }
}

void ACM::updateVertexCMapMap(const std::vector<VertexHandle>& verts, const CMapHandle cm_idx)
{
    for(const auto& vert : verts)
    {
        updateVertexCMapMap(vert, cm_idx);
    }
}

void ACM::updateVertexCMapMap(VertexHandle vert, const CMapHandle cm_idx)
{
    // this may be unecessary
    if(v_cm_map.find(vert) == v_cm_map.end()) // not found
    {
        //create new entry
        v_cm_map[vert];
    }
    v_cm_map[vert] = cm_idx;
}

CMapNeighbour ACM::getCommanCMap(
                              CMapHandle cmh,
                              glm::uvec2 vh1_coords,
                              glm::uvec2 vh2_coords
                              )
{

    CMap* cm_ptr = getCMap(cmh);
    VertexHandle vh1 = cm_ptr->cm.at(vh1_coords.x).at(vh1_coords.y);
    VertexHandle vh2 = cm_ptr->cm.at(vh2_coords.x).at(vh2_coords.y);
    VertexHandle vh1_pair = -1;
    VertexHandle vh2_pair = -1;
    glm::uvec2 vh1_coords_pair;
    glm::uvec2 vh2_coords_pair;
    CMapNeighbour cm_pair;
    CMapHandle cmh_pair = -1;
    for(auto & cm : cm_list)
    {
        if(cm.idx == cmh)
        {
            // skip
            continue;
        }
        else
        {
            for(int i = 0; i < cm.cm.size(); i++)
            {
                for(int j = 0; j < cm.cm.size(); j++)
                {
                    VertexHandle vert = cm.cm.at(i).at(j);
                    if(getVertex(vert)->point == getVertex(vh1)->point)
                    {
                        vh1_pair = vert;
                        vh1_coords_pair.x = i;
                        vh1_coords_pair.y = j;
                    }
                    else if(getVertex(vert)->point == getVertex(vh2)->point)
                    {
                        vh2_pair = vert;
                        vh2_coords_pair.x = i;
                        vh2_coords_pair.y = j;
                    }
                }
            }
            if(( vh1_pair != -1) && ( vh2_pair != -1))
            {
                cmh_pair = cm.idx;
                break;
            }
            else
            {
                // reset vh1_pair and vh2_pair
                vh1_pair = -1;
                vh2_pair = -1;
            }
        }
    }

    NeighbourNumber n = getNeighbourNumber(vh1_coords, vh2_coords);
    CMapOrientation o = getNeighbourCase(n, vh1_coords, vh2_coords, vh1_coords_pair, vh2_coords_pair);
    cm_pair.n = n;
    cm_pair.o = o;
    cm_pair.cmh_pair = cmh_pair;
    return cm_pair;
}

bool ACM::areEdgePointsEqual(const glm::uvec2& e1v1,
                        const glm::uvec2& e1v2,
                        const glm::uvec2& e2v1,
                        const glm::uvec2& e2v2)
{
    if(((e1v1 == e2v1)||(e1v2 == e2v1))
       &&
       ((e1v1 == e2v2)||(e1v2 == e2v2))
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}

NeighbourNumber ACM::getNeighbourNumber(const glm::uvec2& v1, const glm::uvec2& v2)
{
    // neighbour 0
    if(areEdgePointsEqual(v1, v2, glm::uvec2(0,0), glm::uvec2(1,0)))
    {
        return 0;
    }
    // neighbour 1
    else if(areEdgePointsEqual(v1, v2, glm::uvec2(1,0), glm::uvec2(1,1)))
    {
        return 1;
    }
    // neighbour 2
    else if(areEdgePointsEqual(v1, v2, glm::uvec2(1,1), glm::uvec2(0,1)))
    {
        return 2;
    }
    // neighbour 3
    else if(areEdgePointsEqual(v1, v2, glm::uvec2(0,1), glm::uvec2(0,0)))
    {
        return 3;
    }
    // invalid
    else
    {
        return -1;
    }
}

CMapOrientation ACM::getNeighbourCase(int neighbourNum,
                          const glm::uvec2& v1,
                          const glm::uvec2& v2,
                          const glm::uvec2& v1_pair,
                          const glm::uvec2& v2_pair)
{
    CMapOrientation orientation;
    switch (neighbourNum) {
        case 0:
            orientation = getNeighbourCaseFor0(v1_pair, v2_pair);
            break;

        case 1:
            orientation = getNeighbourCaseFor1(v1_pair, v2_pair);
            break;

        case 2:
            orientation = getNeighbourCaseFor2(v1_pair, v2_pair);
            break;

        case 3:
            orientation = getNeighbourCaseFor3(v1_pair, v2_pair);
            break;

        default:
            orientation = CMapOrientation::invalid;
            break;
    }
    return orientation;
}

CMapOrientation ACM::getNeighbourCaseFor0(const glm::uvec2& v1_pair,
                              const glm::uvec2& v2_pair)
{
    // neighbour 0 - same i and j direction
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(0,1), glm::uvec2(1,1)))
    {
        return CMapOrientation::posi_posj;
    }
    // neighbour 0 - i is pos j, j is neg i
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(0,0), glm::uvec2(0,1)))
    {
        return CMapOrientation::posj_negi;
    }
    // neighbour 0 - i is neg i, j is neg j
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(1,0), glm::uvec2(0,0)))
    {
        return CMapOrientation::negi_negj;
    }
    // neighbour 0 - i is neg j, j is pos i
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(1,1), glm::uvec2(1,0)))
    {
        return CMapOrientation::negj_posi;
    }
    // invalid
    return CMapOrientation::invalid;
}

CMapOrientation ACM::getNeighbourCaseFor1(const glm::uvec2& v1_pair,
                                          const glm::uvec2& v2_pair)
{
    // neighbour 1 - same i and j direction
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(0,1), glm::uvec2(0,0)))
    {
        return CMapOrientation::posi_posj;
    }
    // neighbour 1 - i is pos j, j is neg i
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(0,0), glm::uvec2(1,0)))
    {
        return CMapOrientation::posj_negi;
    }
    // neighbour 1 - i is neg i, j is neg j
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(1,0), glm::uvec2(1,1)))
    {
        return CMapOrientation::negi_negj;
    }
    // neighbour 1 - i is neg j, j is pos i
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(1,1), glm::uvec2(0,1)))
    {
        return CMapOrientation::negj_posi;
    }
    // invalid
    return CMapOrientation::invalid;
}

CMapOrientation ACM::getNeighbourCaseFor2(const glm::uvec2& v1_pair,
                                          const glm::uvec2& v2_pair)
{
    // neighbour 0 - same i and j direction
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(0,0), glm::uvec2(1,0)))
    {
        return CMapOrientation::posi_posj;
    }
    // neighbour 0 - i is pos j, j is neg i
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(1,0), glm::uvec2(1,1)))
    {
        return CMapOrientation::posj_negi;
    }
    // neighbour 0 - i is neg i, j is neg j
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(1,1), glm::uvec2(0,1)))
    {
        return CMapOrientation::negi_negj;
    }
    // neighbour 0 - i is neg j, j is pos i
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(0,1), glm::uvec2(0,0)))
    {
        return CMapOrientation::negj_posi;
    }
    // invalid
    return CMapOrientation::invalid;
}

CMapOrientation ACM::getNeighbourCaseFor3(const glm::uvec2& v1_pair,
                                          const glm::uvec2& v2_pair)
{
    // neighbour 0 - same i and j direction
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(1,0), glm::uvec2(1,1)))
    {
        return CMapOrientation::posi_posj;
    }
    // neighbour 0 - i is pos j, j is neg i
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(1,1), glm::uvec2(0,1)))
    {
        return CMapOrientation::posj_negi;
    }
    // neighbour 0 - i is neg i, j is neg j
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(0,1), glm::uvec2(0,0)))
    {
        return CMapOrientation::negi_negj;
    }
    // neighbour 0 - i is neg j, j is pos i
    if(areEdgePointsEqual(v1_pair, v2_pair, glm::uvec2(0,0), glm::uvec2(1,0)))
    {
        return CMapOrientation::negj_posi;
    }
    // invalid
    return CMapOrientation::invalid;
}