#include "acm.hpp"

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
    cm.cm.resize(2);
    for(auto& i : cm.cm)
    {
        i.resize(2);
    }

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
                         VertexHandle vh11,
                         bool isPhantom)
{
    CMap& cm(cm_list.at(cm_idx));
    cm.isPhantom = isPhantom;
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
            verts[2] = cmap->cm.at(i+cmap->vectorScale).at(j+cmap->vectorScale);
            faces.push_back(verts);
            // right triangle
            verts[0] = cmap->cm.at(i).at(j+cmap->vectorScale);
            verts[1] = cmap->cm.at(i).at(j);
            verts[2] = cmap->cm.at(i+cmap->vectorScale).at(j+cmap->vectorScale);
            faces.push_back(verts);
        }
    }
    return faces;
}

void ACM::updateVertexCMapMap(const std::vector<VertexHandle>& verts, const CMapHandle cm_idx)
{
    for(VertexHandle vh : verts)
    {
        // this may be unecessary
        if(v_cm_map.find(vh) == v_cm_map.end()) // not found
        {
            //create new entry
            v_cm_map[vh];
        }
        v_cm_map[vh].push_back(cm_idx);
    }
}