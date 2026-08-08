
#include <networking/NetworkTraffic.h>
#ifndef NETWORKWINDOW_H
#define NETWORKWINDOW_H

class NetworkWindow : public ImGuiWindow {
public:
  void draw() override;
private:
  void drawPacketInspector(int selected);
  bool drawTableEntry(const NetworkEvent& entry, bool selected, int id);
};

#endif // NETWORKWINDOW_H
