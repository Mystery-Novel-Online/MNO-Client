#include "NetworkWindow.h"

#include <networking/NetworkTraffic.h>

void NetworkWindow::draw()
{
  ImGui::Begin("Network Traffic");

  const auto& events = NetworkTrafficLog::instance().events();

  static char filter[256] = {};
  static int selected = -1;
  static bool paused = false;

  if (ImGui::Button("Clear"))
  {
    NetworkTrafficLog::instance().clear();
    selected = -1;
  }

  ImGui::SameLine();

  ImGui::Checkbox("Pause", &paused);

  ImGui::SameLine();

  ImGui::SetNextItemWidth(250);

  ImGui::InputTextWithHint("##filter", "Filter packets...", filter, sizeof(filter));

  ImGui::Separator();

  ImGui::BeginChild("PacketList", ImVec2(300, 0), true);
  {
    if (ImGui::BeginTable("Packets", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp, ImVec2(0, 0)))
    {
      ImGui::TableSetupColumn("Time");
      ImGui::TableSetupColumn("Direction");
      ImGui::TableSetupColumn("Header");
      ImGui::TableSetupColumn("Size");
      ImGui::TableHeadersRow();

      for (int i = 0; i < events.size(); ++i)
      {
        const auto& event = events[i];

        if (paused)
          continue;

        if (filter[0] != '\0')
        {
          const QString filterText = QString::fromUtf8(filter);

          if (!event.header.contains(filterText, Qt::CaseInsensitive))
            continue;
        }

        if(drawTableEntry(event, selected == i, i))
          selected = i;
      }

      ImGui::EndTable();
    }
  }

  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("PacketInspector", ImVec2(330, 0), true);
  {
    drawPacketInspector(selected);
  }

  ImGui::EndChild();

  ImGui::End();
}

void NetworkWindow::drawPacketInspector(int selected)
{
  const auto& events = NetworkTrafficLog::instance().events();

  if (selected < 0 || selected >= events.size())
  {
    ImGui::TextDisabled("Select a packet.");
    return;
  }

  const auto& event = events[selected];

  ImGui::Text("Packet");

  ImGui::Separator();

  ImGui::Text("Header: %s", event.header.toUtf8().constData());
  ImGui::Text("Time: %s", event.timestamp.toString("HH:mm:ss.zzz").toUtf8().constData());
  ImGui::Text("Direction: %s", event.direction == NetworkDirection::Incoming ? "Incoming" : "Outgoing");

  ImGui::Separator();

  ImGui::Text("Content");

  for (int i = 0; i < event.content.size(); ++i)
  {
    ImGui::PushID(i);

    ImGui::Text("[%d]", i);

    ImGui::SameLine();

    ImGui::TextUnformatted(event.content[i].toUtf8().constData());

    ImGui::PopID();
  }

}

bool NetworkWindow::drawTableEntry(const NetworkEvent &entry, bool selected, int id)
{
  bool selectedState = false;

  ImGui::TableNextRow();

  ImGui::TableNextColumn();

  QString time = entry.timestamp.toString("HH:mm:ss.zzz");
  ImGui::TextUnformatted(time.toUtf8().constData());

  ImGui::TableNextColumn();


  if (entry.direction == NetworkDirection::Incoming)
    ImGui::TextUnformatted("IN");
  else
    ImGui::TextUnformatted("OUT");

  ImGui::TableNextColumn();

  ImGui::PushID(id);

  if (ImGui::Selectable(entry.header.toUtf8().constData(), selected, ImGuiSelectableFlags_SpanAllColumns))
    selectedState = true;

  ImGui::PopID();

  ImGui::TableNextColumn();

  ImGui::Text("%d B", entry.raw.toUtf8().size());
  return selectedState;
}
