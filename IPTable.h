#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <cpprest/json.h>
#include <fstream>

using str = std::wstring;

class TableDemoComponent : public Component,
	public TableListBoxModel
{
public:

	TableDemoComponent()
	{
		loadData();
		addAndMakeVisible(table);
		table.setModel(this);
		table.setColour(ListBox::outlineColourId, Colours::grey);
		table.setOutlineThickness(1);
		table.getHeader().addColumn(L"Hostname", 1, 300, 50, 400, TableHeaderComponent::defaultFlags);
		table.getHeader().setStretchToFitActive (true);
		table.setMultipleSelectionEnabled(false);
	}

	int getNumRows() override
	{
		return data.size();
	}

	void paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
	{
		auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
			.interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
		if (rowIsSelected)
			g.fillAll(Colours::lightblue);
		else if (rowNumber % 2)
			g.fillAll(alternateColour);
	}

	void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override
	{
		g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
		g.setFont(font);

		if (rowNumber < data.size())
		{
			std::wstring text = data[rowNumber];
			g.drawText(text.c_str(), 2, 0, width - 4, height, Justification::centredLeft, true);
		}

		g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
		g.fillRect(width - 1, 0, 1, height);
	}

	String getText(const int columnNumber, const int rowNumber) const
	{
		return data[rowNumber].to_wstring().c_str();
	}
	
	struct hostname
	{
		std::wstring name;
		std::wstring addres;
		hostname(const std::wstring& i_name, const std::wstring& i_addr)
			:name{ i_name }, addres{ i_addr } {}
		hostname(const hostname&) = default;
		hostname(hostname&&) = default;
		hostname() = default;
		hostname& operator=(const hostname&) = default;
		hostname& operator=(hostname&&) = default;

		std::wstring to_wstring() const
		{
			return name + L" [" + addres + L"]";
		}

		operator std::wstring()
		{
			return to_wstring();
		}
	};

	virtual void selectedRowsChanged(int last_row) override
	{
		on_selected_row_changed(last_row);
	}

	TableListBox table;     // the table component itself
	std::function<void(int)> on_selected_row_changed = [](int) {};

	hostname operator[](const int row) { if (row >= data.size() || row < 0) return hostname(L"", L""); else return data[row]; }
	std::vector<hostname> data;

private:

	Font font{ 14.0f };
	
	void loadData()
	{
		using namespace web;
		std::wifstream file{ R"(C:\Users\raidg\Documents\hosts.json)" };
		str whole{ L"" };
		str line;
		while (std::getline(file, line))
			whole += line;
		file.close();
		json::value dat = json::value::parse(whole);
		const int length = dat[L"len"].as_integer();
		data.reserve(length);
		for (int i = 0; i < length; i++)
		{
			json::value tmp = dat[L"hostnames"].as_array()[i];
			data.emplace_back(hostname(tmp[L"name"].as_string(), tmp[L"address"].as_string()));
		}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TableDemoComponent)
};
