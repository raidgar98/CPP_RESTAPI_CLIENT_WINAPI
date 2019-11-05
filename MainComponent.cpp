/*
  ==============================================================================

	This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "SpecifyRefresher.h"
#include "GlobalRefresher.h"


//==============================================================================
MainComponent::MainComponent()
{
	setSize(window_width, window_height);

	//Global Labels
	add_std_label(L"Œrednie Obci¹¿enie Procesora", margin_left + 0, margin_top + 0);
	add_std_label(L"Œrednie U¿ycie Pamiêci", margin_left + 0, margin_top + 25);
	add_std_label(L"Œrednie U¿ycie Dysków", margin_left + 0, margin_top + 50);

	//Global Progress Bars
	globalProcessor.setBounds(margin_left + width(labels[0]) + 20, margin_top + Y(labels[0]) - 5, window_width - width(labels[0]) - default_height - 10 - margin_left, default_height);
	globalRAM.setBounds(margin_left + width(labels[1]) + 20, margin_top + Y(labels[1]) - 5, window_width - width(labels[1]) - default_height - 10 - margin_left, default_height);
	globalDisk.setBounds(margin_left + width(labels[2]) + 20, margin_top + Y(labels[2]) - 5, window_width - width(labels[2]) - default_height - 10 - margin_left, default_height);
	addAndMakeVisible(globalProcessor);
	addAndMakeVisible(globalRAM);
	addAndMakeVisible(globalDisk);

	//Table config
	tbl.table.setBounds(
		margin_left,
		margin_top + Y(globalDisk) + height(globalDisk) + margin_top,
		0.2 * window_width,
		window_height - Y(globalDisk) - ( 4 * margin_top ) - ( 2 * default_height )
	);
	tbl.on_selected_row_changed = [&](int row) { update_specific(tbl[row].addres); };
	addAndMakeVisible(tbl.table);

	//Setting up global refresher
	for (int i = 0; i < tbl.getNumRows(); i++)
		addresses.push_back(tbl[i].addres);
	if (tbl.data.size() > 0) global_refresher.reset(new std::thread(GlobalRefresher(this)));

	const int alt_left_margin = margin_left + X(tbl.table) + width(tbl.table);
	const int alt_top_margin = margin_top + Y(tbl.table);

	//Specify Labels
	const int s_model_proc = add_std_label(L"Model Procesora:", alt_left_margin, alt_top_margin);
	const int s_cores = add_std_label(L"Iloœæ Rdzenii Procesora:", alt_left_margin, alt_top_margin + 25);
	const int s_processor_usage = add_std_label(L"Obci¹¿enie Procesora:", alt_left_margin, alt_top_margin + 50);
	const int s_ram_capacity = add_std_label(L"Ca³kowita Pojemnoœæ Pamiêci Fizycznej:", alt_left_margin, alt_top_margin + 75);
	const int s_ram_usage = add_std_label(L"U¿ycie Pamiêci Fizycznej:", alt_left_margin, alt_top_margin + 100);
	const int s_os = add_std_label(L"System Operacyjny:", alt_left_margin, alt_top_margin + 125);
	const int s_os_ver = add_std_label(L"Wersja Systemu Operacyjnego:", alt_left_margin, alt_top_margin + 150);
	const int s_sp_ver = add_std_label(L"Wersja Service Pack'a Systemu Operacyjnego", alt_left_margin, alt_top_margin + 175);
	const int s_ip = add_std_label(L"Dostêpne Adresy IP:", alt_left_margin, alt_top_margin + 200);
	const int s_choose_disk = add_std_label(L"Wybierz Dysk:", alt_left_margin, alt_top_margin + 225);
	const int s_disk_capacity = add_std_label(L"Pojemnoœæ Dysku:", alt_left_margin, alt_top_margin + 250);
	const int s_disk_free = add_std_label(L"Zajêta Przestrzeñ:", alt_left_margin, alt_top_margin + 275);

	//Specify TextEditors
	configure_txt_editor(specProcModel, s_model_proc, L"Intel Core i5");
	configure_txt_editor(specProcCores, s_cores, L"8");
	configure_txt_editor(specRamCapacity, s_ram_capacity, L"12 G");
	configure_txt_editor(specOS, s_os, L"Windows 10");
	configure_txt_editor(specOSVersion, s_os_ver, L"1900.12");
	configure_txt_editor(specOSSPVersion, s_sp_ver, L"2.1");
	configure_txt_editor(specIP, s_ip, L"127.0.0.1; 192.168.56.1");
	configure_txt_editor(specDiskCapacity, s_disk_capacity, L"1.0 T");

	specProcessorUsage.setBounds(X(labels[s_processor_usage]) + margin_left + width(labels[s_processor_usage]),
		Y(labels[s_processor_usage]),
		window_width - X(labels[s_processor_usage]) - width(labels[s_processor_usage]) - (2 * margin_left),
		default_height);
	addAndMakeVisible(specProcessorUsage);

	specRamUsage.setBounds(X(labels[s_ram_usage]) + margin_left + width(labels[s_ram_usage]),
		Y(labels[s_ram_usage]),
		window_width - X(labels[s_ram_usage]) - width(labels[s_ram_usage]) - (2 * margin_left),
		default_height);
	addAndMakeVisible(specRamUsage);

	specDiskFree.setBounds(X(labels[s_disk_free]) + margin_left + width(labels[s_disk_free]),
		Y(labels[s_disk_free]),
		window_width - X(labels[s_disk_free]) - width(labels[s_disk_free]) - (2 * margin_left),
		default_height);
	addAndMakeVisible(specDiskFree);
	
	specDisks.setBounds(
		X(labels[s_choose_disk]) + margin_left + width(labels[s_choose_disk]),
		Y(labels[s_choose_disk]),
		window_width - X(labels[s_choose_disk]) - width(labels[s_choose_disk]) - (2 * margin_left),
		default_height
	);
	addAndMakeVisible(specDisks);

	//Address Buttons
	rem_address.onClick = [&]()
	{
		int selected = tbl.table.getSelectedRow();
		if (selected == -1) return;
		auto it = tbl.data.begin();
		for (int i = 0; i < selected; i++)
			it++;
		tbl.data.erase(it);

		system(R"(ERASE C:\Users\raidg\Documents\hosts.json)");
		json::value serial = json::value::object();
		serial[L"len"] = json::value::number(tbl.data.size());
		std::vector<json::value> vec;
		vec.reserve(tbl.data.size());
		for (const TableDemoComponent::hostname& var : tbl.data)
		{
			json::value tmp = json::value::object();
			tmp[L"name"] = json::value::string(var.name);
			tmp[L"address"] = json::value::string(var.addres);
			vec.push_back(tmp);
		}
		serial[L"hostnames"] = json::value::array(vec);

		std::wofstream out{ R"(C:\Users\raidg\Documents\hosts.json)" };
		out << serial.serialize();
		out.close();

		tbl.table.updateContent();

		if (global_refresher.get() != nullptr)
		{
			while (!global_run.try_lock()) {}
			global_refresher->join();
			global_run.unlock();
			global_refresher.reset(nullptr);
		}

		if(tbl.data.size() > 0) global_refresher.reset(new std::thread(GlobalRefresher(this)));
	};
	rem_address.setBounds(X(tbl.table), Y(tbl.table) + height(tbl.table) + margin_top, width(tbl.table) / 2, default_height);
	rem_address.setButtonText(L"Usuñ");
	addAndMakeVisible(rem_address);

}

MainComponent::~MainComponent()
{
	while (!global_run.try_lock()) {}
	global_refresher->join();
	global_run.unlock();
	while (!specify_run.try_lock()) {}
	specify_refresher->join();
	specify_run.unlock();
}

int MainComponent::add_std_label(const std::wstring& caption, const int pos_x, const int pos_y, int width, int height)
{
	assert(amount_of_labels < max_amount_of_labels);
	Label& tmp = labels[amount_of_labels];
	tmp.setText(String(caption.c_str()), NotificationType::dontSendNotification);
	tmp.setFont(std_font);
	tmp.setBounds(pos_x, pos_y, width == -1 ? caption.length() * 7.5 : width, height == -1 ? default_height : height);
	tmp.setJustificationType(Justification::centredLeft);
	addAndMakeVisible(tmp);
	amount_of_labels++;
	return amount_of_labels - 1;
}

void MainComponent::configure_txt_editor(TextEditor& src, const int idx, const std::wstring& txt)
{
	src.setBounds(
		X(labels[idx]) + margin_left + width(labels[idx]),
		Y(labels[idx]),
		window_width - X(labels[idx]) - width(labels[idx]) - (2 * margin_left),
		default_height
	);
	src.setReadOnly(true);
	//src.setText(txt.c_str());
	src.setFont(std_font);
	src.setJustification(Justification::centred);
	addAndMakeVisible(src);
}

void MainComponent::update_specific(const std::wstring& src)
{
	if (specify_refresher.get() != nullptr)
	{
		while (!specify_run.try_lock()) {}
		specify_refresher->join();
		specify_run.unlock();
		specify_refresher.reset(nullptr);
	}

	if (src == L"") return;

	specify_refresher.reset(new std::thread(SpecifyRefresher(this, src)));
}

//==============================================================================