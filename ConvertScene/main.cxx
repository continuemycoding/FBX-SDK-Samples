/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/////////////////////////////////////////////////////////////////////////
//
// This program converts any file in a format supported by the FBX SDK 
// into DAE, FBX, 3DS, OBJ and DXF files. 
//
// Steps:
// 1. Initialize SDK objects.
// 2. Load a file(fbx, obj,...) to a FBX scene.
// 3. Create a exporter.
// 4. Retrieve the writer ID according to the description of file format.
// 5. Initialize exporter with specified file format
// 6. Export.
// 7. Destroy the exporter
// 8. Destroy the FBX SDK manager
//
/////////////////////////////////////////////////////////////////////////

#include <fbxsdk.h>

void ProcessNode(FbxNode* pNode)
{
    FBXSDK_printf(pNode->GetName());
    FBXSDK_printf("\n");

    if (pNode->GetNodeAttribute())
    {
        switch (pNode->GetNodeAttribute()->GetAttributeType())
        {
        case FbxNodeAttribute::eMesh:
            //ProcessMesh(pNode);
            FBXSDK_printf("eMesh.\n");
            break;

        case FbxNodeAttribute::eSkeleton:
            //ProcessSkeleton(pNode);
            FBXSDK_printf("eSkeleton.\n");
            break;

        case FbxNodeAttribute::eLight:
            //ProcessLight(pNode);
            FBXSDK_printf("eLight.\n");
            break;

        case FbxNodeAttribute::eCamera:
            //ProcessCamera();
            FBXSDK_printf("eCamera.\n");
            break;

        default:
            FBXSDK_printf("default\n");
            break;
        }
    }

    if (pNode == NULL)
        return;

    for (int i = 0; i < pNode->GetChildCount(); ++i)
    {
        ProcessNode(pNode->GetChild(i));
    }
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        FBXSDK_printf("fbxPath rename/remove name, [name2]\n");
        return 0;
    }

    FbxString fbxPath(argv[1]);

    FbxManager* pManager = FbxManager::Create();

    //Create an IOSettings object. This object holds all import/export settings.
    FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(ios);

    //Load plugins from the executable directory (optional)
    pManager->LoadPluginsDirectory(FbxGetApplicationDirectory().Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    FbxScene* pScene = FbxScene::Create(pManager, "My Scene");

    // Create an importer.
    FbxImporter* pImporter = FbxImporter::Create(pManager, "");

    // Initialize the importer by providing a filename.
    pImporter->Initialize(fbxPath.Buffer(), -1, pManager->GetIOSettings());

    // Import the scene.
    pImporter->Import(pScene);

    // Destroy the importer.
    pImporter->Destroy();

    //ProcessNode(pScene->GetRootNode());

    if (FbxString(argv[2]) == FbxString("rename"))
    {
        for (int i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
        {
            FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);

            if (FbxString(pAnimStack->GetName()) == FbxString(argv[3]))
            {
                pAnimStack->SetName(argv[4]);
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
        {
            FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);

            if (FbxString(pAnimStack->GetName()) == FbxString(argv[3]))
            {
                pAnimStack->Destroy();
                break;
            }
        }
    }

    //pAnimStack->LocalStart = (FbxTime)pAnimStack->LocalStop / 2;
    //pAnimStack->LocalStop = (FbxTime)pAnimStack->LocalStart + (FbxTime)pAnimStack->LocalStop / 4;

    // Retrieve the writer ID according to the description of file format.
    int lFormat = pManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX binary (*.fbx)");

    // Create an exporter.
    FbxExporter* pExporter = FbxExporter::Create(pManager, "");

    // Initialize the exporter.
    pExporter->Initialize(fbxPath.Left(fbxPath.GetLen() - 4) + "_out.fbx", lFormat, pManager->GetIOSettings());
    // Export the scene.
    pExporter->Export(pScene);

    // Destroy the exporter.
    pExporter->Destroy();

    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
    pManager->Destroy();

    return 0;
}

