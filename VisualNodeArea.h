#pragma once

#include "VisualNodeFactory.h"

#define VISUAL_NODE_GRID_STEP 15.0f

class VisualNodeSystem;

// Type of events for general callbacks.
enum VISUAL_NODE_EVENT
{
	VISUAL_NODE_REMOVED = 0,
	VISUAL_NODE_DESTROYED = 1,
	VISUAL_NODE_BEFORE_CONNECTED = 2,
	VISUAL_NODE_AFTER_CONNECTED = 3,
	VISUAL_NODE_BEFORE_DISCONNECTED = 4,
	VISUAL_NODE_AFTER_DISCONNECTED = 5
};

class VisualNodeArea
{
	friend VisualNodeSystem;

public:
	VisualNodeArea();
	~VisualNodeArea();

	ImVec2 GetAreaPosition() const;
	void SetAreaPosition(ImVec2 NewValue);

	ImVec2 GetAreaSize() const;
	void SetAreaSize(ImVec2 NewValue);

	ImVec2 GetAreaRenderOffset() const;
	void SetAreaRenderOffset(ImVec2 Offset);

	float GetZoomFactor() const;
	void SetZoomFactor(float NewValue);

	VisualNode* GetHovered() const;
	std::vector<VisualNode*> GetSelected();

	std::vector<VisualNode*> GetNodesByName(std::string NodeName) const;
	std::vector<VisualNode*> GetNodesByType(std::string NodeType) const;

	bool IsMouseHovered() const;
	bool IsAreaFillingWindow();
	void SetIsAreaFillingWindow(bool NewValue);
	int GetNodeCount() const;

	void Update();
	void Clear();
	void Reset();
	void AddNode(VisualNode* NewNode);
	void DeleteNode(const VisualNode* Node);
	void SetMainContextMenuFunc(void(*Func)());
	void PropagateUpdateToConnectedNodes(const VisualNode* CallerNode) const;
	void SetNodeEventCallback(void(*Func)(VisualNode*, VISUAL_NODE_EVENT));
	std::string ToJson() const;
	void SaveToFile(const char* FileName) const;
	void LoadFromFile(const char* FileName);
	void SaveNodesToFile(const char* FileName, std::vector<VisualNode*> Nodes);
	void RunOnEachNode(void(*Func)(VisualNode*));
	void RunOnEachConnectedNode(VisualNode* StartNode, void(*Func)(VisualNode*));
	void ClearSelection();
	void GetAllNodesAABB(ImVec2& Min, ImVec2& Max) const;
	ImVec2 GetAllNodesAABBCenter() const;
	ImVec2 GetRenderedViewCenter() const;
	bool TryToConnect(const VisualNode* OutNode, size_t OutNodeSocketIndex, const VisualNode* InNode, size_t InNodeSocketIndex);
	bool TryToConnect(const VisualNode* OutNode, std::string OutSocketID, const VisualNode* InNode, std::string InSocketID);
	bool TriggerSocketEvent(NodeSocket* CallerNodeSocket, NodeSocket* TriggeredNodeSocket, VISUAL_NODE_SOCKET_EVENT EventType);
	bool TriggerOrphanSocketEvent(VisualNode* Node, VISUAL_NODE_SOCKET_EVENT EventType);

	static VisualNodeArea* CreateNodeArea(std::vector<VisualNode*> Nodes);
	static VisualNodeArea* FromJson(std::string JsonText);
	static void CopyNodesTo(VisualNodeArea* SourceNodeArea, VisualNodeArea* TargetNodeArea);
	static bool IsNodeIDInList(std::string ID, std::vector<VisualNode*> List);
	static bool EmptyOrFilledByNulls(const std::vector<VisualNode*> Vector);

private:
	struct SocketEvent
	{
		NodeSocket* TriggeredNodeSocket;
		NodeSocket* CallerNodeSocket;
		VISUAL_NODE_SOCKET_EVENT EventType;
	};

	// Background grid variables.
	float GRID_SIZE = 10000.0f;
	int BOLD_LINE_FREQUENCY = 10;
	float DEFAULT_LINE_WIDTH = 1;
	float BOLD_LINE_WIDTH = 3;

	float Zoom = 1.0f;
	void ApplyZoom(float Delta);
	float MAX_ZOOM_LEVEL = 5.0f;  // Max zoom 500%
	float MIN_ZOOM_LEVEL = 0.2f;  // Min zoom 20%
	float GetNodeSocketSize() const { return NODE_SOCKET_SIZE * Zoom; }
	float GetNodeTitleHeight() const { return NODE_TITLE_HEIGHT * Zoom; }
	ImVec2 GetMouseDragDelta() const { return ImGui::GetMouseDragDelta(0) * Zoom; }
	ImVec2 GetMouseDelta() const { return ImGui::GetIO().MouseDelta / Zoom; }
	float GetConnectionThickness() const { return 3.0f * Zoom; }

	static ImVec2 NeededShift;

	bool bClearing = false;
	bool bFillWindow = false;
	ImDrawList* CurrentDrawList = nullptr;
	ImGuiWindow* NodeAreaWindow = nullptr;
	std::vector<VisualNode*> Nodes;
	VisualNode* Hovered;
	bool bMouseHovered = false;
	std::vector<VisualNode*> Selected;
	ImVec2 MouseCursorPosition;
	ImVec2 MouseCursorSize = ImVec2(1, 1);
	ImVec2 MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	ImVec2 MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);
	ImGuiWindow* MouseDownIn = nullptr;
	bool bOpenMainContextMenu = false;
	std::vector<VisualNodeConnection*> Connections;
	NodeSocket* SocketLookingForConnection = nullptr;
	NodeSocket* SocketHovered = nullptr;
	ImVec2 AreaPosition;
	ImVec2 AreaSize;
	ImU32 GridBackgroundColor = IM_COL32(32, 32, 32, 255);
	ImVec4 GridLinesColor = ImVec4(53.0f / 255.0f, 53.0f / 255.0f, 53.0f / 255.0f, 0.5f);
	ImVec4 GridBoldLinesColor = ImVec4(27.0f / 255.0f, 27.0f / 255.0f, 27.0f / 255.0f, 1.0f);
	ImVec2 RenderOffset = ImVec2(0.0, 0.0);
	void(*MainContextMenuFunc)() = nullptr;
	std::vector<void(*)(VisualNode*, VISUAL_NODE_EVENT)> NodeEventsCallbacks;
	std::queue<SocketEvent> SocketEventQueue;

	bool AddSelected(VisualNode* NewNode);
	bool IsSelected(const VisualNode* Node) const;
	void InputUpdateNode(VisualNode* Node);
	void InputUpdateSocket(NodeSocket* Socket);
	void RenderNode(VisualNode* Node) const;
	void RenderNodeSockets(const VisualNode* Node) const;
	void RenderNodeSocket(NodeSocket* Socket) const;
	void DrawHermiteLine(ImVec2 P1, ImVec2 P2, int Steps, ImColor Color, VisualNodeConnectionStyle* Style) const;
	void RenderConnection(const VisualNodeConnection* Connection) const;
	ImVec2 SocketToPosition(const NodeSocket* Socket) const;
	std::vector<VisualNodeConnection*> GetAllConnections(const NodeSocket* Socket) const;
	void Disconnect(VisualNodeConnection*& Connection);
	void InputUpdate();
	void Render();
	void RenderGrid(ImVec2 CurrentPosition) const;
	void PropagateNodeEventsCallbacks(VisualNode* Node, VISUAL_NODE_EVENT EventToPropagate) const;
	void ProcessSocketEventQueue();
};