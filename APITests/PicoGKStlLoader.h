//
// SPDX-License-Identifier: CC0-1.0
//
// This example code file is released to the public under Creative Commons CC0.
// See https://creativecommons.org/publicdomain/zero/1.0/legalcode
//
// To the extent possible under law, LEAP 71 has waived all copyright and
// related or neighboring rights to this PicoGK Example Code.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef PICOGKSTLLOADER_H_
#define PICOGKSTLLOADER_H_

#include <string>
#include <limits>
#include <vector>
#include <fstream>

namespace PicoGKStl
{

namespace
{
#pragma pack(1)

#pragma pack(1)
struct Vertex
{
    float X;
    float Y;
    float Z;
};

struct StlItem
{
    Vertex sNormal;
    Vertex A;
    Vertex B;
    Vertex C;
    uint16_t ushAttributes;
};

struct Triangle
{
    uint32_t A;
    uint32_t B;
    uint32_t C;
};
#pragma pack()
}

template <typename TVertex, typename TTriangle>
bool bReadStlFile(  const std::string&      strFileName,
                    std::vector<TVertex>*   poVertices,
                    std::vector<TTriangle>* poTriangles)
{
    static_assert(sizeof(TVertex) == sizeof(Vertex));
    static_assert(sizeof(TTriangle) == sizeof(TTriangle));
    
    std::ifstream oFile(    strFileName,
                        std::ios::binary | std::ios::ate);
    
    if (!oFile.is_open())
        return false;
    
    oFile.seekg(0, std::ios::end);
    std::streamsize size = oFile.tellg();
    oFile.seekg(0, std::ios::beg);
    
    if (size < (80 + 4 + sizeof(StlItem)))
    {
        return false;
    }
    
    std::vector<char> oBuffer(size);
    
    if (!oFile.read(oBuffer.data(), size))
        return false;
    
    int32_t iTriangles = *(int*)(oBuffer.data() + 80);
    
    if (iTriangles <= 0 || size < 80 + 4 + iTriangles * sizeof(StlItem))
        return false;
    
    StlItem* psItems = (StlItem*)(oBuffer.data() + 80 + 4);
    
    for (uint32_t n = 0; n < iTriangles; n++)
    {
        TVertex* pvecA = (TVertex*) &(psItems[n].A);
        TVertex* pvecB = (TVertex*) &(psItems[n].B);
        TVertex* pvecC = (TVertex*) &(psItems[n].C);
        
        Triangle sTri;
        sTri.A = (uint32_t) poVertices->size();
        sTri.B = (uint32_t) sTri.A+1;
        sTri.C = (uint32_t) sTri.A+2;
        
        poVertices->push_back(*pvecA);
        poVertices->push_back(*pvecB);
        poVertices->push_back(*pvecC);
        
        TTriangle* psTri = (TTriangle*) &sTri;
        
        poTriangles->push_back(*psTri);
    }
    
    return true;
}

}
#endif
