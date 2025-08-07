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

#include "../API/PicoGK.h"
#include <iostream>
#include "PicoGKStlLoader.h"
#include <thread>
#include <assert.h>

// Change this to run tests
#define TESTFILE_PATH "/Users/yourusername/PicoGK_Testfiles"

PKVIEWER g_hViewer = nullptr;

bool bLoadFileIntoBuffer(   std::string strFilePath,
                            std::vector<char>** ppoBuffer)
{
    std::ifstream oIn(  strFilePath,
                        std::ios_base::in | std::ios_base::binary);
    
    if (!oIn)
        return false;
    
    // Determine size
    oIn.seekg(0, std::ios::end);
        
    // Get the position, which will be the size of the file
    std::streampos nSize = oIn.tellg();
    // Seek back to the beginning
    oIn.seekg(0, std::ios::beg);
    
    std::vector<char>* poBuffer = new std::vector<char>(nSize);
    
    oIn.read(poBuffer->data(), nSize);
    *ppoBuffer = poBuffer;
    return true;
}



void LogInfo(   const char* pszMessage,
                bool bFatalError)
{
    std::cout << "Received a message: " << pszMessage << " - FatalError=" << (bFatalError ? "true" : "false") << "\n";
}

void SetIdentityMatrix(PKMatrix4x4* pmat)
{
    if (pmat != nullptr)
    {
        pmat->vec1 = {1.0f, 0.0f, 0.0f, 0.0f};
        pmat->vec2 = {0.0f, 1.0f, 0.0f, 0.0f};
        pmat->vec3 = {0.0f, 0.0f, 1.0f, 0.0f};
        pmat->vec4 = {0.0f, 0.0f, 0.0f, 1.0f};
    }
}

void UpdateRequested(   PKVIEWER            hViewer,
                        const PKVector2*    pvecViewport,
                        PKColorFloat*       pclrBackground,
                        PKMatrix4x4*        pmatViewProjection,
                        PKVector3*          pvecEyePosition)
{
    assert(hViewer == g_hViewer);
    
    pclrBackground->B = ((float)(rand() % 10000)) / 10000.0f;
    pclrBackground->R = ((float)(rand() % 10000)) / 10000.0f;
    pclrBackground->G = ((float)(rand() % 10000)) / 10000.0f;
    pclrBackground->A = 0.0f;
    
    static PKMatrix4x4 mat;
    mat.vec1.X = -0.6971824f;
    mat.vec1.Y = -0.4841640f;
    mat.vec1.Z = -0.641527176f;
    mat.vec1.W = -0.640152335f;
    
    mat.vec2.X = 0.680555344f;
    mat.vec2.Y = -0.495992988f;
    mat.vec2.Z = -0.657200813f;
    mat.vec2.W = -0.655792356f;
    
    mat.vec3.X = 0.0f;
    mat.vec3.Y = 1.58731735f;
    mat.vec3.Z = -0.401035994f;
    mat.vec3.W = -0.400176555f;
    
    mat.vec4.X = 0.340745032f;
    mat.vec4.Y = -6.47347879f;
    mat.vec4.Z = 33.2254829f;
    mat.vec4.W = 33.2542801f;
    
    *pmatViewProjection    = mat;
    
    PKVector3 vecEye;
    vecEye.X = 20.493336f;
    vecEye.Y = 20.493336f;
    vecEye.Z = 16.732738f;
    
    *pvecEyePosition = vecEye;
}

void KeyPressed(        PKVIEWER    hViewer,
                        int32_t     iKey,
                        int32_t     iScancode,
                        int32_t     iAction,
                        int32_t     iModifiers)
{
    assert(hViewer == g_hViewer);
    
    if (iAction != 1)
        return;
    
    if (iKey == 256)
            
    {
        // ESC Pressed
        Viewer_RequestClose(hViewer);
    }
        
    if (iKey ==  258)
    {
        Viewer_RequestScreenShot(hViewer, TESTFILE_PATH"/SShot.tga");
    }
}

void MouseMoved(    PKVIEWER            hViewer,
                    const PKVector2*    vecMousePos,
                    bool                bShift,
                    bool                bCtrl,
                    bool                bAlt,
                    bool                bCmd)
{
    assert(hViewer == g_hViewer);
    
    Viewer_RequestUpdate(hViewer);
}

void ScrollWheel(   PKVIEWER            hViewer,
                    const PKVector2*    vecOffset,
                    const PKVector2*    vecMousePos,
                    bool                bShift,
                    bool                bCtrl,
                    bool                bAlt,
                    bool                bCmd)
{
    assert(hViewer == g_hViewer);
    
    Viewer_RequestUpdate(hViewer);
}

void WindowSize(    PKVIEWER            hViewer,
                    const PKVector2*    vecSize)
{
    assert(hViewer == g_hViewer);
    Viewer_RequestUpdate(hViewer);
}

void MouseButton(   PKVIEWER    hViewer,
                    int32_t     iButton,
                    int32_t     iAction,
                    int32_t     iModifiers,
                    const       PKVector2* vecMousePos)
{
    assert(hViewer == g_hViewer);
}


int main(int argc, const char * argv[])
{
    char pszInfo[PKINFOSTRINGLEN];
    
    Library_GetName(pszInfo);
    std::cout << pszInfo << " ";
    
    Library_GetVersion(pszInfo);
    std::cout << pszInfo << "\n";
    
    Library_GetBuildInfo(pszInfo);
    std::cout << pszInfo << "\n";
    
    PKINSTANCE hLib = Library_hCreateInstance(1.0f);
    
    PKMESH hMesh = Mesh_hCreate(hLib);
    assert(Mesh_bIsValid(hLib, hMesh));
    
    std::vector<PKVector3>  oVertices;
    std::vector<PKTriangle> oTriangles;
    
    std::cout << "Current Mem Usage " << Library_nTotalMemUsage(hLib) / 1024 << " KB\n";
    
    if (!PicoGKStl::bReadStlFile(  TESTFILE_PATH"/Teapot.stl",
                                    &oVertices,
                                    &oTriangles))
    {
        std::cout << "Failed to load STL from" << TESTFILE_PATH << "\n";;
    }
    else
    {
        std::cout << "Mesh with " << oVertices.size() << " vertices\n";
        for (auto vec : oVertices)
        {
            Mesh_nAddVertex(hLib, hMesh, &vec);
        }
        
        for (auto tri : oTriangles)
        {
            Mesh_nAddTriangle(hLib, hMesh, &tri);
        }
    }
    
    std::cout << "Current Mem Usage " << Library_nTotalMemUsage(hLib) / 1024 << " KB\n";
    
    PKVector2 vecSize;
    vecSize.X = 2048;
    vecSize.Y = 2048;
    
    g_hViewer = Viewer_hCreate(
                    "Hello PicoGK",
                    &vecSize,
                    LogInfo,
                    UpdateRequested,
                    KeyPressed,
                    MouseMoved,
                    MouseButton,
                    ScrollWheel,
                    WindowSize);
    
    if (!Viewer_bIsValid(g_hViewer))
        return 99;
    
    std::vector<char>* poDiffuse    = nullptr;
    std::vector<char>* poSpecular   = nullptr;
    
    if (    bLoadFileIntoBuffer(TESTFILE_PATH"/Diffuse.dds", &poDiffuse) &&
            bLoadFileIntoBuffer(TESTFILE_PATH"/Specular.dds", &poSpecular))
    {
        Viewer_bLoadLightSetup( g_hViewer,
                               poDiffuse->data(),
                               (int32_t) poDiffuse->size(),
                               poSpecular->data(),
                               (int32_t) poSpecular->size());
    }
    else
    {
        std::cout << "Failed to load viewer textures from " << TESTFILE_PATH << "\n";
    }
            
    PKVOXELS hVoxels = Voxels_hCreate(hLib);
    assert(Voxels_bIsValid(hLib, hVoxels));
    Voxels_RenderMesh(hLib, hVoxels, hMesh);
    
    PKVector3 vecSearch;
    vecSearch.X = 50;
    vecSearch.Y = 50;
    vecSearch.Z = 50;
    
    PKVector3 vecSurface;
    
    if (Voxels_bClosestPointOnSurface(hLib, hVoxels, &vecSearch, &vecSurface))
    {
        PKColorFloat clr;
        clr.R = 1.0f;
        clr.G = 0.0f;
        clr.B = 0.0f;
        clr.A = 1.0f;
        
        PKPOLYLINE hPoly = PolyLine_hCreate(hLib, &clr);
        
        PolyLine_nAddVertex(hLib, hPoly, &vecSearch);
        PolyLine_nAddVertex(hLib, hPoly, &vecSurface);
        
        Viewer_AddPolyLine(hLib, g_hViewer, 0, hPoly);
    }
    
    std::cout << "Current Mem Usage " << Library_nTotalMemUsage(hLib) / 1024 << " KB\n";
    
    PKVDBFILE hVdb = VdbFile_hCreate(hLib);
    
    int nIndex = VdbFile_nAddVoxels(hLib,hVdb, "Voxels", hVoxels);
    
    if (!VdbFile_bSaveToFile(hLib, hVdb, TESTFILE_PATH"/Voxels.vdb"))
    {
        std::cout << "Failed to save Vdb to " << TESTFILE_PATH << "\n";
    }
    
    std::cout << "Current Mem Usage " << Library_nTotalMemUsage(hLib) / 1024 << " KB\n";
    
    VdbFile_Destroy(hLib, hVdb);
    
    std::cout << "Current Mem Usage " << Library_nTotalMemUsage(hLib) / 1024 << " KB\n";
    
    PKVDBFILE   hVdbRead    = VdbFile_hCreateFromFile(hLib, TESTFILE_PATH"/Voxels.vdb");
    PKVOXELS    hVoxelsRead = hVoxels;
    if (hVdbRead == 0)
    {
        std::cout << "Failed to read Vdb from " << TESTFILE_PATH << "\n";
    }
    else
    {
        hVoxelsRead = VdbFile_hGetVoxels(hLib, hVdbRead, nIndex);
        VdbFile_Destroy(hLib, hVdbRead);
    }
    
    std::cout << "Current Mem Usage " << Library_nTotalMemUsage(hLib) / 1024 << " KB\n";

    Viewer_AddVoxels(hLib, g_hViewer, 0, hVoxelsRead);
    
    // Try again, which should replace them
    Viewer_AddVoxels(hLib, g_hViewer, 0, hVoxelsRead);
    
    Viewer_RequestUpdate(g_hViewer);
    
    while (Viewer_bPoll(g_hViewer))
    {
        std::this_thread::yield();
    }
    
    std::cout << "Current Mem Usage " << Library_nTotalMemUsage(hLib) / 1024 << " KB\n";
    
    Viewer_RemoveVoxels(hLib, g_hViewer, hVoxelsRead);
    
    PKLATTICE hLattice = Lattice_hCreate(hLib);
    assert(Lattice_bIsValid(hLib, hLattice));
    Lattice_Destroy(hLib, hLattice);
    
    Mesh_Destroy(hLib, hMesh);
    Voxels_Destroy(hLib, hVoxels);
    
    if (hVoxelsRead != hVoxels)
        Voxels_Destroy(hLib, hVoxelsRead);
    
    std::cout << "Current Mem Usage " << Library_nTotalMemUsage(hLib) / 1024 << " KB\n";
    
    return 0;
}
