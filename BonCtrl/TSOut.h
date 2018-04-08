#pragma once

#include <Windows.h>

#include "../Common/StructDef.h"
#include "../Common/PathUtil.h"
#include "../Common/StringUtil.h"
#include "../Common/ErrDef.h"
#include "../Common/EpgDataCap3Util.h"
#include "../Common/TSPacketUtil.h"
#include "../Common/ThreadUtil.h"

#include "BonCtrlDef.h"
#include "ScrambleDecoderUtil.h"
#include "CreatePATPacket.h"
#include "OneServiceUtil.h"
#include "PMTUtil.h"
#include "CATUtil.h"
#include <functional>

class CTSOut
{
public:
	CTSOut(void);
	~CTSOut(void);

	void SetChChangeEvent(BOOL resetEpgUtil = FALSE);
	BOOL IsChUnknown(DWORD* elapsedTime = NULL);

	//現在のストリームのIDを取得する
	//戻り値：
	// TRUE（成功）、FALSE（失敗）
	//引数：
	// ONID		[OUT]originalNetworkID
	// TSID		[OUT]transportStreamID
	BOOL GetStreamID(
		WORD* ONID,
		WORD* TSID
		);

	void AddTSBuff(BYTE* data, DWORD dataSize);

	//EMM処理の動作設定
	//戻り値：
	// TRUE（成功）、FALSE（失敗）
	//引数：
	// enable		[IN] TRUE（処理する）、FALSE（処理しない）
	BOOL SetEmm(
		BOOL enable
		);

	//EMM処理を行った数
	//戻り値：
	// 処理数
	DWORD GetEmmCount();

	//DLLのロード状態を取得
	//戻り値：
	// TRUE（ロードに成功している）、FALSE（ロードに失敗している）
	//引数：
	// loadErrDll		[OUT]ロードに失敗したDLLファイル名
	BOOL GetLoadStatus(
		wstring& loadErrDll
		);

	//EPGデータの保存を開始する
	//戻り値：
	// TRUE（成功）、FALSE（失敗）
	BOOL StartSaveEPG(
		const wstring& epgFilePath_
		);

	//EPGデータの保存を終了する
	//引数：
	// copy			[IN]tmpからコピー処理行うかどうか
	void StopSaveEPG(
		BOOL copy
		);

	//EPGデータの蓄積状態を取得する
	//戻り値：
	// ステータス
	//引数：
	// l_eitFlag		[IN]L-EITのステータスを取得
	EPG_SECTION_STATUS GetSectionStatus(
		BOOL l_eitFlag
		);

	//指定サービスのEPGデータの蓄積状態を取得する
	//戻り値：
	// ステータス,取得できたかどうか
	//引数：
	// originalNetworkID		[IN]取得対象のOriginalNetworkID
	// transportStreamID		[IN]取得対象のTransportStreamID
	// serviceID				[IN]取得対象のServiceID
	// l_eitFlag				[IN]L-EITのステータスを取得
	pair<EPG_SECTION_STATUS, BOOL> GetSectionStatusService(
		WORD originalNetworkID,
		WORD transportStreamID,
		WORD serviceID,
		BOOL l_eitFlag
		);

	//自ストリームのサービス一覧を取得する
	//戻り値：
	// エラーコード
	//引数：
	// funcGetList		[IN]戻り値がNO_ERRのときサービス情報の個数とそのリストを引数として呼び出される関数
	DWORD GetServiceListActual(
		const std::function<void(DWORD, SERVICE_INFO*)>& funcGetList
		);

	//TSストリーム制御用コントロールを作成する
	//戻り値：
	// 制御識別ID
	//引数：
	// sendUdpTcp	[IN]UDP/TCP送信用にする
	DWORD CreateServiceCtrl(
		BOOL sendUdpTcp
		);

	//TSストリーム制御用コントロールを削除する
	//戻り値：
	// TRUE（成功）、FALSE（失敗
	//引数：
	// id			[IN]制御識別ID
	BOOL DeleteServiceCtrl(
		DWORD id
		);

	//制御対象のサービスを設定する
	//戻り値：
	// TRUE（成功）、FALSE（失敗
	//引数：
	// id			[IN]制御識別ID
	// serviceID	[IN]対象サービスID、0xFFFFで全サービス対象
	BOOL SetServiceID(
		DWORD id,
		WORD serviceID
		);

	BOOL GetServiceID(
		DWORD id,
		WORD* serviceID
		);

	//UDPで送信を行う
	//戻り値：
	// TRUE（成功）、FALSE（失敗）
	//引数：
	// id			[IN]制御識別ID
	// sendList		[IN/OUT]送信先リスト。NULLで停止。Portは実際に送信に使用したPortが返る。
	BOOL SendUdp(
		DWORD id,
		vector<NW_SEND_INFO>* sendList
		);

	//TCPで送信を行う
	//戻り値：
	// TRUE（成功）、FALSE（失敗）
	//引数：
	// id			[IN]制御識別ID
	// sendList		[IN/OUT]送信先リスト。NULLで停止。Portは実際に送信に使用したPortが返る。
	BOOL SendTcp(
		DWORD id,
		vector<NW_SEND_INFO>* sendList
		);

	//ファイル保存を開始する
	//戻り値：
	// TRUE（成功）、FALSE（失敗）
	//引数：
	// id					[IN]制御識別ID
	// fileName				[IN]保存ファイルパス
	// overWriteFlag		[IN]同一ファイル名存在時に上書きするかどうか（TRUE：する、FALSE：しない）
	// pittariFlag			[IN]ぴったりモード（TRUE：する、FALSE：しない）
	// pittariONID			[IN]ぴったりモードで録画するONID
	// pittariTSID			[IN]ぴったりモードで録画するTSID
	// pittariSID			[IN]ぴったりモードで録画するSID
	// pittariEventID		[IN]ぴったりモードで録画するイベントID
	// createSize			[IN]ファイル作成時にディスクに予約する容量
	// saveFolder			[IN]使用するフォルダ一覧
	// saveFolderSub		[IN]HDDの空きがなくなった場合に一時的に使用するフォルダ
	BOOL StartSave(
		DWORD id,
		const wstring& fileName,
		BOOL overWriteFlag,
		BOOL pittariFlag,
		WORD pittariONID,
		WORD pittariTSID,
		WORD pittariSID,
		WORD pittariEventID,
		ULONGLONG createSize,
		const vector<REC_FILE_SET_INFO>& saveFolder,
		const vector<wstring>& saveFolderSub,
		int maxBuffCount
	);

	//ファイル保存を終了する
	//戻り値：
	// TRUE（成功）、FALSE（失敗）
	//引数：
	// id			[IN]制御識別ID
	BOOL EndSave(
		DWORD id
		);

	//スクランブル解除処理の動作設定
	//戻り値：
	// TRUE（成功）、FALSE（失敗）
	//引数：
	// enable		[IN] TRUE（処理する）、FALSE（処理しない）
	BOOL SetScramble(
		DWORD id,
		BOOL enable
		);

	//字幕とデータ放送含めるかどうか
	//引数：
	// id					[IN]制御識別ID
	// enableCaption		[IN]字幕を TRUE（含める）、FALSE（含めない）
	// enableData			[IN]データ放送を TRUE（含める）、FALSE（含めない）
	void SetServiceMode(
		DWORD id,
		BOOL enableCaption,
		BOOL enableData
		);

	//エラーカウントをクリアする
	void ClearErrCount(
		DWORD id
		);

	//ドロップとスクランブルのカウントを取得する
	//引数：
	// drop				[OUT]ドロップ数
	// scramble			[OUT]スクランブル数
	void GetErrCount(
		DWORD id,
		ULONGLONG* drop,
		ULONGLONG* scramble
		);

	//録画中のファイルの出力サイズを取得する
	//引数：
	// id					[IN]制御識別ID
	// writeSize			[OUT]出力サイズ
	void GetRecWriteSize(
		DWORD id,
		__int64* writeSize
		);

	//指定サービスの現在or次のEPG情報を取得する
	//戻り値：
	// エラーコード
	//引数：
	// originalNetworkID		[IN]取得対象のoriginalNetworkID
	// transportStreamID		[IN]取得対象のtransportStreamID
	// serviceID				[IN]取得対象のServiceID
	// nextFlag					[IN]TRUE（次の番組）、FALSE（現在の番組）
	// epgInfo					[OUT]EPG情報
	DWORD GetEpgInfo(
		WORD originalNetworkID,
		WORD transportStreamID,
		WORD serviceID,
		BOOL nextFlag,
		EPGDB_EVENT_INFO* epgInfo
		);
	
	//指定イベントのEPG情報を取得する
	//戻り値：
	// エラーコード
	//引数：
	// originalNetworkID		[IN]取得対象のoriginalNetworkID
	// transportStreamID		[IN]取得対象のtransportStreamID
	// serviceID				[IN]取得対象のServiceID
	// eventID					[IN]取得対象のEventID
	// pfOnlyFlag				[IN]p/fからのみ検索するかどうか
	// epgInfo					[OUT]EPG情報
	DWORD SearchEpgInfo(
		WORD originalNetworkID,
		WORD transportStreamID,
		WORD serviceID,
		WORD eventID,
		BYTE pfOnlyFlag,
		EPGDB_EVENT_INFO* epgInfo
		);

	//PC時計を元としたストリーム時間との差を取得する
	//戻り値：
	// 差の秒数
	int GetTimeDelay(
		);
	
	//録画中かどうか
	//戻り値：
	// TRUE（録画中）、FALSE（していない）
	BOOL IsRec();

	//録画中のファイルのファイルパスを取得する
	//引数：
	// id					[IN]制御識別ID
	// filePath				[OUT]保存ファイル名
	// subRecFlag			[OUT]サブ録画が発生したかどうか
	void GetSaveFilePath(
		DWORD id,
		wstring* filePath,
		BOOL* subRecFlag
		);

	//ドロップとスクランブルのカウントを保存する
	//引数：
	// id					[IN]制御識別ID
	// filePath				[IN]保存ファイル名
	void SaveErrCount(
		DWORD id,
		const wstring& filePath
		);

	void SetSignalLevel(
		float signalLv
		);

	void SetBonDriver(
		const wstring& bonDriver
		);

	void SetNoLogScramble(
		BOOL noLog
		);

	void SetParseEpgPostProcess(
		BOOL parsePost
		);
protected:
	//objLock->epgUtilLockの順にロックする
	recursive_mutex_ objLock;
	recursive_mutex_ epgUtilLock;

	CEpgDataCap3Util epgUtil;
	CScrambleDecoderUtil decodeUtil;
	CCreatePATPacket patUtil;

	enum { CH_ST_INIT, CH_ST_WAIT_PAT, CH_ST_WAIT_PAT2, CH_ST_WAIT_ID, CH_ST_DONE } chChangeState;
	DWORD chChangeTime;
	WORD lastONID;
	WORD lastTSID;

	vector<BYTE> decodeBuff;

	BOOL enableDecodeFlag;
	BOOL emmEnableFlag;
	BOOL serviceOnlyFlag;

	map<DWORD, std::unique_ptr<COneServiceUtil>> serviceUtilMap; //キー識別ID
	map<WORD, CPMTUtil> pmtUtilMap; //キーPMTのPID
	CCATUtil catUtil;

	vector<WORD> needPIDList;

	DWORD nextCtrlID;

	std::unique_ptr<FILE, decltype(&fclose)> epgFile;
	enum { EPG_FILE_ST_NONE, EPG_FILE_ST_PAT, EPG_FILE_ST_TOT, EPG_FILE_ST_ALL } epgFileState;
	__int64 epgFileTotPos;
	wstring epgFilePath;
	wstring epgTempFilePath;

	wstring bonFile;
	BOOL noLogScramble;
	BOOL parseEpgPostProcess;
protected:
	void ParseEpgPacket(BYTE* data, const CTSPacketUtil& packet);

	void CheckNeedPID();

	DWORD GetNextID();

	void ResetErrCount();

	void OnChChanged(WORD onid, WORD tsid);
};

