//*****************************************************************************************//
//**                                                                                     **//
//**                   　　　　　    Main                                                **//
//**                                                                                     **//
//*****************************************************************************************//

#include "../../Common/Window/Win.h"
#include "../../Common/Direct3DWrapper/DxText.h"
#include "../../CNN/PPMLoader.h"
#include "GenerativeAdversarialNetworks.h"
#pragma comment(lib,"winmm.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	HWND hWnd;
	MSG msg;
	//DirectX12ラッパー
	Dx12Process* dx;
	//文字入力
	DxText* text;

	Createwindow(&hWnd, hInstance, nCmdShow, 900, 600, L"ImageDetection");

	//Dx12Processオブジェクト生成
	Dx12Process::InstanceCreate();
	//Dx11Processオブジェクト取得
	dx = Dx12Process::GetInstance();
	dx->Initialize(hWnd, 900, 600);
	DxText::InstanceCreate();
	text = DxText::GetInstance();
	Control* control;
	PPMLoader* ppm = nullptr;
	control = Control::GetInstance();

	dx->Bigin(0);
	GenerativeAdversarialNetworks* gan = nullptr;
	gan = new GenerativeAdversarialNetworks(200, 64, 64, 1);
	dx->End(0);
	dx->WaitFenceCurrent();

	if (!gan->LoadDataSet()) {
		//ppm = new PPMLoader(L"../gazou/faceData/*", 64, 64, GRAYSCALE);
		ppm = new PPMLoader(L"../gazou/MNIST Dataset JPG format/*", 64, 64, GRAYSCALE);
		gan->CreateLearningImagebyte(ppm->GetFileNum(), ppm->GetImageArr());
	}

	while (1) {//アプリ実行中ループ
		if (!DispatchMSG(&msg))break;
		Directionkey key = control->Direction();
		T_float::GetTime(hWnd);
		switch (key) {
		case UP:
			break;
		case DOWN:
			break;
		case ENTER:
			break;
		case CANCEL:
			break;
		}

		gan->Com();

		dx->Bigin(0);
		dx->Sclear(0);

		gan->Draw();

		text->UpDate();
		text->Draw(0);
		dx->End(0);
		dx->WaitFenceCurrent();
		dx->DrawScreen();
	}
	gan->SaveDataSet();
	S_DELETE(gan);
	S_DELETE(ppm);
	DxText::DeleteInstance();
	Dx12Process::DeleteInstance();
	return (int)msg.wParam;
}