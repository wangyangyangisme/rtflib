#include "errors.h"
#include "globals.h"
#include "rtflib.h"



RTF_DOCUMENT_FORMAT rtfDocFormat;					// RTF document formatting params
RTF_SECTION_FORMAT rtfSecFormat;					// RTF section formatting params
RTF_PARAGRAPH_FORMAT rtfParFormat;					// RTF paragraph formatting params
RTF_TABLEROW_FORMAT rtfRowFormat;					// RTF table row formatting params
RTF_TABLECELL_FORMAT rtfCellFormat;					// RTF table cell formatting params



// RTF library global params
FILE* rtfFile = NULL;
char rtfFontTable[4096] = "";
char rtfColorTable[4096] = "";
IPicture* rtfPicture = NULL;



// Creates new RTF document
int rtf_open( char* filename, char* fonts, char* colors )
{
	// Set error flag
	int error = RTF_SUCCESS;

	// Initialize global params
	rtf_init();

	// Set RTF document font table
	if ( fonts != NULL )
	{
		if ( strcmp( fonts, "" ) != 0 )
			rtf_set_fonttable(fonts);
	}

	// Set RTF document color table
	if ( colors != NULL )
	{
		if ( strcmp( colors, "" ) != 0 )
			rtf_set_colortable(colors);
	}

	// Create RTF document
	rtfFile = fopen( filename, "w" );

	if ( rtfFile != NULL )
	{
		// Write RTF document header
		if ( !rtf_write_header() )
			error = RTF_HEADER_ERROR;

		// Write RTF document formatting properties
		if ( !rtf_write_documentformat() )
			error = RTF_DOCUMENTFORMAT_ERROR;

		// Create first RTF document section with default formatting
		rtf_write_sectionformat();
	}
	else
		error = RTF_OPEN_ERROR;

	// Return error flag
	return error;
}


// Closes created RTF document
int rtf_close()
{
	// Set error flag
	int error = RTF_SUCCESS;

	// Free IPicture object
	if ( rtfPicture != NULL )
	{
		rtfPicture->Release();
		rtfPicture = NULL;
	}

	// Write RTF document end part
	char rtfText[1024];
	strcpy( rtfText, "\n\\par}" );
	fwrite( rtfText, 1, strlen(rtfText), rtfFile );

	// Close RTF document
	if ( fclose(rtfFile) )
		error = RTF_CLOSE_ERROR;

	// Return error flag
	return error;
}


// Writes RTF document header
bool rtf_write_header()
{
	// Set error flag
	bool result = true;

	// Standard RTF document header
	char rtfText[1024];
	strcpy( rtfText, "{\\rtf1\\ansi\\ansicpg1252\\deff0{\\fonttbl" );
	strcat( rtfText, rtfFontTable );
	strcat( rtfText, "}{\\colortbl" );
	strcat( rtfText, rtfColorTable );
	strcat( rtfText, "}{\\*\\generator rtflib ver. 1.0;}" );
	strcat( rtfText, "\n{\\info{\\author rtflib ver. 1.0}{\\company ETC Company LTD.}}" );

	// Writes standard RTF document header part
	if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
		result = false;

	// Return error flag
	return result;
}


// Sets global RTF library params
void rtf_init()
{
	// Set RTF document default font table
	strcpy( rtfFontTable, "" );
	strcat( rtfFontTable, "{\\f0\\froman\\fcharset0\\cpg1252 Times New Roman}" );
	strcat( rtfFontTable, "{\\f1\\fswiss\\fcharset0\\cpg1252 Arial}" );
	strcat( rtfFontTable, "{\\f2\\fmodern\\fcharset0\\cpg1252 Courier New}" );
	strcat( rtfFontTable, "{\\f3\\fscript\\fcharset0\\cpg1252 Cursive}" );
	strcat( rtfFontTable, "{\\f4\\fdecor\\fcharset0\\cpg1252 Old English}" );
	strcat( rtfFontTable, "{\\f5\\ftech\\fcharset0\\cpg1252 Symbol}" );
	strcat( rtfFontTable, "{\\f6\\fbidi\\fcharset0\\cpg1252 Miriam}" );

	// Set RTF document default color table
	strcpy( rtfColorTable, "" );
	strcat( rtfColorTable, "\\red0\\green0\\blue0;" );
	strcat( rtfColorTable, "\\red255\\green0\\blue0;" );
	strcat( rtfColorTable, "\\red0\\green255\\blue0;" );
	strcat( rtfColorTable, "\\red0\\green0\\blue255;" );
	strcat( rtfColorTable, "\\red255\\green255\\blue0;" );
	strcat( rtfColorTable, "\\red255\\green0\\blue255;" );
	strcat( rtfColorTable, "\\red0\\green255\\blue255;" );
	strcat( rtfColorTable, "\\red255\\green255\\blue255;" );
	strcat( rtfColorTable, "\\red128\\green0\\blue0;" );
	strcat( rtfColorTable, "\\red0\\green128\\blue0;" );
	strcat( rtfColorTable, "\\red0\\green0\\blue128;" );
	strcat( rtfColorTable, "\\red128\\green128\\blue0;" );
	strcat( rtfColorTable, "\\red128\\green0\\blue128;" );
	strcat( rtfColorTable, "\\red0\\green128\\blue128;" );
	strcat( rtfColorTable, "\\red128\\green128\\blue128;" );

	// Set default formatting
	rtf_set_defaultformat();
}


// Sets default RTF document formatting
void rtf_set_defaultformat()
{
	// Set default RTF document formatting properties
	RTF_DOCUMENT_FORMAT df = {RTF_DOCUMENTVIEWKIND_PAGE, 100, 12240, 15840, 1800, 1800, 1440, 1440, false, 0, false};
	rtf_set_documentformat(&df);

	// Set default RTF section formatting properties
	RTF_SECTION_FORMAT sf = {RTF_SECTIONBREAK_CONTINUOUS, false, true, 12240, 15840, 1800, 1800, 1440, 1440, 0, 720, 720, false, 720, 720, false, 1, 720, false};
	rtf_set_sectionformat(&sf);

	// Set default RTF paragraph formatting properties
	RTF_PARAGRAPH_FORMAT pf = {RTF_PARAGRAPHBREAK_NONE, false, true, RTF_PARAGRAPHALIGN_LEFT, 0, 0, 0, 0, 0, 0, "", false, false, false, false, false, false};
	pf.BORDERS.borderColor = 0;
	pf.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
	pf.BORDERS.borderSpace = 0;
	pf.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
	pf.BORDERS.borderWidth = 0;
	pf.CHARACTER.animatedCharacter = false;
	pf.CHARACTER.backgroundColor = 0;
	pf.CHARACTER.boldCharacter = false;
	pf.CHARACTER.capitalCharacter = false;
	pf.CHARACTER.doublestrikeCharacter = false;
	pf.CHARACTER.embossCharacter = false;
	pf.CHARACTER.engraveCharacter = false;
	pf.CHARACTER.expandCharacter = 0;
	pf.CHARACTER.fontNumber = 0;
	pf.CHARACTER.fontSize = 24;
	pf.CHARACTER.foregroundColor = 0;
	pf.CHARACTER.italicCharacter = false;
	pf.CHARACTER.kerningCharacter = 0;
	pf.CHARACTER.outlineCharacter = false;
	pf.CHARACTER.scaleCharacter = 100;
	pf.CHARACTER.shadowCharacter = false;
	pf.CHARACTER.smallcapitalCharacter = false;
	pf.CHARACTER.strikeCharacter = false;
	pf.CHARACTER.subscriptCharacter = false;
	pf.CHARACTER.superscriptCharacter = false;
	pf.CHARACTER.underlineCharacter = 0;
	pf.NUMS.numsChar = char(0x95);
	pf.NUMS.numsLevel = 11;
	pf.NUMS.numsSpace = 360;
	pf.SHADING.shadingBkColor = 0;
	pf.SHADING.shadingFillColor = 0;
	pf.SHADING.shadingIntensity = 0;
	pf.SHADING.shadingType = RTF_PARAGRAPHSHADINGTYPE_FILL;
	pf.TABS.tabKind = RTF_PARAGRAPHTABKIND_NONE;
	pf.TABS.tabLead = RTF_PARAGRAPHTABLEAD_NONE;
	pf.TABS.tabPosition = 0;
	rtf_set_paragraphformat(&pf);

	// Set default RTF table row formatting properties
	RTF_TABLEROW_FORMAT rf = {RTF_ROWTEXTALIGN_LEFT, 0, 0, 0, 0, 0, 0};
	rtf_set_tablerowformat(&rf);

	// Set default RTF table cell formatting properties
	RTF_TABLECELL_FORMAT cf = {RTF_CELLTEXTALIGN_CENTER, 0, 0, 0, 0, RTF_CELLTEXTDIRECTION_LRTB, false};
	cf.SHADING.shadingBkColor = 0;
	cf.SHADING.shadingFillColor = 0;
	cf.SHADING.shadingIntensity = 0;
	cf.SHADING.shadingType = RTF_PARAGRAPHSHADINGTYPE_FILL;
	cf.borderBottom.border = false;
	cf.borderBottom.BORDERS.borderColor = 0;
	cf.borderBottom.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
	cf.borderBottom.BORDERS.borderSpace = 0;
	cf.borderBottom.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
	cf.borderBottom.BORDERS.borderWidth = 5;
	cf.borderLeft.border = false;
	cf.borderLeft.BORDERS.borderColor = 0;
	cf.borderLeft.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
	cf.borderLeft.BORDERS.borderSpace = 0;
	cf.borderLeft.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
	cf.borderLeft.BORDERS.borderWidth = 5;
	cf.borderRight.border = false;
	cf.borderRight.BORDERS.borderColor = 0;
	cf.borderRight.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
	cf.borderRight.BORDERS.borderSpace = 0;
	cf.borderRight.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
	cf.borderRight.BORDERS.borderWidth = 5;
	cf.borderTop.border = false;
	cf.borderTop.BORDERS.borderColor = 0;
	cf.borderTop.BORDERS.borderKind = RTF_PARAGRAPHBORDERKIND_NONE;
	cf.borderTop.BORDERS.borderSpace = 0;
	cf.borderTop.BORDERS.borderType = RTF_PARAGRAPHBORDERTYPE_STHICK;
	cf.borderTop.BORDERS.borderWidth = 5;
	rtf_set_tablecellformat(&cf);
}


// Sets new RTF document font table
void rtf_set_fonttable( char* fonts )
{
	// Clear old font table
	strcpy( rtfFontTable, "" );

	// Set separator list
	char separator[] = ";";

	// Create new RTF document font table
	int font_number = 0;
	char font_table_entry[1024];
	char* token = strtok( fonts, separator );
 	while ( token != NULL )
	{
		// Format font table entry
		sprintf( font_table_entry, "{\\f%d\\fnil\\fcharset0\\cpg1252 %s}", font_number, token );
		strcat( rtfFontTable, font_table_entry );

		// Get next font
		token = strtok( NULL, separator );
		font_number++;
	}
}


// Sets new RTF document color table
void rtf_set_colortable(char* colors)
{
	// Clear old color table
	strcpy( rtfColorTable, "" );

	// Set separator list
	char separator[] = ";";

	// Create new RTF document color table
	int color_number = 0;
	char color_table_entry[1024];
	char* token = strtok( colors, separator );
 	while ( token != NULL )
	{
		// Red
		sprintf( color_table_entry, "\\red%s", token );
		strcat( rtfColorTable, color_table_entry );

		// Green
		token = strtok( NULL, separator );
		if ( token != NULL )
		{
			sprintf( color_table_entry, "\\green%s", token );
			strcat( rtfColorTable, color_table_entry );
		}

		// Blue
		token = strtok( NULL, separator );
		if ( token != NULL )
		{
			sprintf( color_table_entry, "\\blue%s;", token );
			strcat( rtfColorTable, color_table_entry );
		}

		// Get next color
		token = strtok( NULL, separator );
		color_number++;
	}
}


// Sets RTF document formatting properties
void rtf_set_documentformat(RTF_DOCUMENT_FORMAT* df)
{
	// Set new RTF document formatting properties
	memcpy( &rtfDocFormat, df, sizeof(RTF_DOCUMENT_FORMAT) );
}


// Writes RTF document formatting properties
bool rtf_write_documentformat()
{
	// Set error flag
	bool result = true;

	// RTF document text
	char rtfText[1024];
	strcpy( rtfText, "" );

	sprintf( rtfText, "\\viewkind%d\\viewscale%d\\paperw%d\\paperh%d\\margl%d\\margr%d\\margt%d\\margb%d\\gutter%d", 
		rtfDocFormat.viewKind, rtfDocFormat.viewScale, rtfDocFormat.paperWidth, rtfDocFormat.paperHeight,
		rtfDocFormat.marginLeft, rtfDocFormat.marginRight, rtfDocFormat.marginTop, rtfDocFormat.marginBottom, rtfDocFormat.gutterWidth );

	if ( rtfDocFormat.facingPages )
		strcat( rtfText, "\\facingp" );
	if ( rtfDocFormat.readOnly )
		strcat( rtfText, "\\annotprot" );

	// Writes RTF document formatting properties
	if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
		result = false;

	// Return error flag
	return result;
}


// Sets RTF section formatting properties
void rtf_set_sectionformat(RTF_SECTION_FORMAT* sf)
{
	// Set new RTF section formatting properties
	memcpy( &rtfSecFormat, sf, sizeof(RTF_SECTION_FORMAT) );
}


// Writes RTF section formatting properties
bool rtf_write_sectionformat()
{
	// Set error flag
	bool result = true;

	// RTF document text
	char rtfText[1024];
	strcpy( rtfText, "" );

	// Format new section
	char text[1024]="", pgn[100]="";
	if ( rtfSecFormat.newSection )
		strcat( text, "\\sect" );
	if ( rtfSecFormat.defaultSection )
		strcat( text, "\\sectd" );
	if ( rtfSecFormat.showPageNumber )
	{
		sprintf( pgn, "\\pgnx%d\\pgny%d", rtfSecFormat.pageNumberOffsetX, rtfSecFormat.pageNumberOffsetY );
		strcat( text, pgn );
	}
	
	// Format section break
	char sbr[100] = "";
	switch (rtfSecFormat.sectionBreak)
	{
		// Continuous break
		case RTF_SECTIONBREAK_CONTINUOUS:
			strcat( sbr, "\\sbknone" );
			break;

		// Column break
		case RTF_SECTIONBREAK_COLUMN:
			strcat( sbr, "\\sbkcol" );
			break;

		// Page break
		case RTF_SECTIONBREAK_PAGE:
			strcat( sbr, "\\sbkpage" );
			break;

		// Even-page break
		case RTF_SECTIONBREAK_EVENPAGE:
			strcat( sbr, "\\sbkeven" );
			break;

		// Odd-page break
		case RTF_SECTIONBREAK_ODDPAGE:
			strcat( sbr, "\\sbkodd" );
			break;
	}

	// Format section columns
	char cols[100] = "";
	if ( rtfSecFormat.cols == true )
	{
		// Format columns
		sprintf( cols, "\\cols%d\\colsx%d", rtfSecFormat.colsNumber, rtfSecFormat.colsDistance );

		if ( rtfSecFormat.colsLineBetween )
			strcat( cols, "\\linebetcol" );
	}

	sprintf( rtfText, "\n%s%s%s\\pgwsxn%d\\pghsxn%d\\marglsxn%d\\margrsxn%d\\margtsxn%d\\margbsxn%d\\guttersxn%d\\headery%d\\footery%d", 
		text, sbr, cols, rtfSecFormat.pageWidth, rtfSecFormat.pageHeight, rtfSecFormat.pageMarginLeft, rtfSecFormat.pageMarginRight,
		rtfSecFormat.pageMarginTop, rtfSecFormat.pageMarginBottom, rtfSecFormat.pageGutterWidth, rtfSecFormat.pageHeaderOffset, rtfSecFormat.pageFooterOffset );

	// Writes RTF section formatting properties
	if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
		result = false;

	// Return error flag
	return result;
}


// Starts new RTF section
int rtf_start_section()
{
	// Set error flag
	int error = RTF_SUCCESS;

	// Set new section flag
	rtfSecFormat.newSection = true;

	// Starts new RTF section
	if( !rtf_write_sectionformat() )
		error = RTF_SECTIONFORMAT_ERROR;

	// Return error flag
	return error;
}


// Sets RTF paragraph formatting properties
void rtf_set_paragraphformat(RTF_PARAGRAPH_FORMAT* pf)
{
	// Set new RTF paragraph formatting properties
	memcpy( &rtfParFormat, pf, sizeof(RTF_PARAGRAPH_FORMAT) );
}


// Writes RTF paragraph formatting properties
bool rtf_write_paragraphformat()
{
	// Set error flag
	bool result = true;

	// RTF document text
	char rtfText[4096];
	strcpy( rtfText, "" );

	// Format new paragraph
	char text[1024] = "";
	if ( rtfParFormat.newParagraph )
		strcat( text, "\\par" );
	if ( rtfParFormat.defaultParagraph )
		strcat( text, "\\pard" );
	if ( rtfParFormat.tableText == false )
		strcat( text, "\\plain" );
	else
		strcat( text, "\\intbl" );
	
	switch (rtfParFormat.paragraphBreak)
	{
		// No break
		case RTF_PARAGRAPHBREAK_NONE:
			break;

		// Page break;
		case RTF_PARAGRAPHBREAK_PAGE:
			strcat( text, "\\page" );
			break;

		// Column break;
		case RTF_PARAGRAPHBREAK_COLUMN:
			strcat( text, "\\column" );
			break;

		// Line break;
		case RTF_PARAGRAPHBREAK_LINE:
			strcat( text, "\\line" );
			break;
	}

	// Format aligment
	switch (rtfParFormat.paragraphAligment)
	{
		// Left aligned paragraph
		case RTF_PARAGRAPHALIGN_LEFT:
			strcat( text, "\\ql" );
			break;

		// Center aligned paragraph
		case RTF_PARAGRAPHALIGN_CENTER:
			strcat( text, "\\qc" );
			break;

		// Right aligned paragraph
		case RTF_PARAGRAPHALIGN_RIGHT:
			strcat( text, "\\qr" );
			break;

		// Justified aligned paragraph
		case RTF_PARAGRAPHALIGN_JUSTIFY:
			strcat( text, "\\qj" );
			break;
	}

	// Format tabs
	if ( rtfParFormat.paragraphTabs == true )
	{
		// Set tab kind
		switch ( rtfParFormat.TABS.tabKind )
		{
			// No tab
			case RTF_PARAGRAPHTABKIND_NONE:
				break;

			// Centered tab
			case RTF_PARAGRAPHTABKIND_CENTER:
				strcat( text, "\\tqc" );
				break;

			// Flush-right tab
			case RTF_PARAGRAPHTABKIND_RIGHT:
				strcat( text, "\\tqr" );
				break;

			// Decimal tab
			case RTF_PARAGRAPHTABKIND_DECIMAL:
				strcat( text, "\\tqdec" );
				break;
		}

		// Set tab leader
		switch ( rtfParFormat.TABS.tabLead )
		{
			// No lead
			case RTF_PARAGRAPHTABLEAD_NONE:
				break;

			// Leader dots
			case RTF_PARAGRAPHTABLEAD_DOT:
				strcat( text, "\\tldot" );
				break;

			// Leader middle dots
			case RTF_PARAGRAPHTABLEAD_MDOT:
				strcat( text, "\\tlmdot" );
				break;

			// Leader hyphens
			case RTF_PARAGRAPHTABLEAD_HYPH:
				strcat( text, "\\tlhyph" );
				break;

			// Leader underline
			case RTF_PARAGRAPHTABLEAD_UNDERLINE:
				strcat( text, "\\tlul" );
				break;

			// Leader thick line
			case RTF_PARAGRAPHTABLEAD_THICKLINE:
				strcat( text, "\\tlth" );
				break;

			// Leader equal sign
			case RTF_PARAGRAPHTABLEAD_EQUAL:
				strcat( text, "\\tleq" );
				break;
		}

		// Set tab position
		char tb[10];
		sprintf( tb, "\\tx%d", rtfParFormat.TABS.tabPosition );
		strcat( text, tb );
	}

	// Format bullets and numbering
	if ( rtfParFormat.paragraphNums == true )
	{
		char nums[1024];
		sprintf( nums, "{\\*\\pn\\pnlvl%d\\pnsp%d\\pntxtb %c}", rtfParFormat.NUMS.numsLevel, rtfParFormat.NUMS.numsSpace, rtfParFormat.NUMS.numsChar );
		strcat( text, nums );
	}

	// Format paragraph borders
	if ( rtfParFormat.paragraphBorders == true )
	{
		char border[1024] = "";

		// Format paragraph border kind
		switch (rtfParFormat.BORDERS.borderKind)
		{
			// No border
			case RTF_PARAGRAPHBORDERKIND_NONE:
				break;

			// Border top
			case RTF_PARAGRAPHBORDERKIND_TOP:
				strcat( border, "\\brdrt" );
				break;

			// Border bottom
			case RTF_PARAGRAPHBORDERKIND_BOTTOM:
				strcat( border, "\\brdrb" );
				break;

			// Border left
			case RTF_PARAGRAPHBORDERKIND_LEFT:
				strcat( border, "\\brdrl" );
				break;

			// Border right
			case RTF_PARAGRAPHBORDERKIND_RIGHT:
				strcat( border, "\\brdrr" );
				break;

			// Border box
			case RTF_PARAGRAPHBORDERKIND_BOX:
				strcat( border, "\\box" );
				break;
		}

		// Format paragraph border type
		char *br = rtf_get_bordername(rtfParFormat.BORDERS.borderType);
		strcat( border, br );

		// Set paragraph border width
		char brd[100];
		sprintf( brd, "\\brdrw%d\\brsp%d", rtfParFormat.BORDERS.borderWidth, rtfParFormat.BORDERS.borderSpace );
		strcat( border, brd );
		strcat( text, border );

		// Set paragraph border color
		char brdcol[100];
		sprintf( brdcol, "\\brdrcf%d", rtfParFormat.BORDERS.borderColor );
		strcat( text, brdcol );
	}

	// Format paragraph shading
	if ( rtfParFormat.paragraphShading == true )
	{
		char shading[100];
		sprintf( shading, "\\shading%d", rtfParFormat.SHADING.shadingIntensity );

		// Format paragraph shading
		char* sh = rtf_get_shadingname( rtfParFormat.SHADING.shadingType, false );
		strcat( text, sh );

		// Set paragraph shading color
		char shcol[100];
		sprintf( shcol, "\\cfpat%d\\cbpat%d", rtfParFormat.SHADING.shadingFillColor, rtfParFormat.SHADING.shadingBkColor );
		strcat( text, shcol );
	}

	// Format paragraph font
	char font[1024] = "";
	sprintf( font, "\\animtext%d\\expndtw%d\\kerning%d\\charscalex%d\\f%d\\fs%d\\cf%d", rtfParFormat.CHARACTER.animatedCharacter, 
		rtfParFormat.CHARACTER.expandCharacter, rtfParFormat.CHARACTER.kerningCharacter, rtfParFormat.CHARACTER.scaleCharacter, 
		rtfParFormat.CHARACTER.fontNumber, rtfParFormat.CHARACTER.fontSize, rtfParFormat.CHARACTER.foregroundColor );
	if ( rtfParFormat.CHARACTER.boldCharacter )
		strcat( font, "\\b" );
	else
		strcat( font, "\\b0" );
	if ( rtfParFormat.CHARACTER.capitalCharacter )
		strcat( font, "\\caps" );
	else
		strcat( font, "\\caps0" );
	if ( rtfParFormat.CHARACTER.doublestrikeCharacter )
		strcat( font, "\\striked1" );
	else
		strcat( font, "\\striked0" );
	if ( rtfParFormat.CHARACTER.embossCharacter )
		strcat( font, "\\embo" );
	if ( rtfParFormat.CHARACTER.engraveCharacter )
		strcat( font, "\\impr" );
	if ( rtfParFormat.CHARACTER.italicCharacter )
		strcat( font, "\\i" );
	else
		strcat( font, "\\i0" );
	if ( rtfParFormat.CHARACTER.outlineCharacter )
		strcat( font, "\\outl" );
	else
		strcat( font, "\\outl0" );
	if ( rtfParFormat.CHARACTER.shadowCharacter )
		strcat( font, "\\shad" );
	else
		strcat( font, "\\shad0" );
	if ( rtfParFormat.CHARACTER.smallcapitalCharacter )
		strcat( font, "\\scaps" );
	else
		strcat( font, "\\scaps0" );
	if ( rtfParFormat.CHARACTER.strikeCharacter )
		strcat( font, "\\strike" );
	else
		strcat( font, "\\strike0" );
	if ( rtfParFormat.CHARACTER.subscriptCharacter )
		strcat( font, "\\sub" );
	if ( rtfParFormat.CHARACTER.superscriptCharacter )
		strcat( font, "\\super" );
	
	switch (rtfParFormat.CHARACTER.underlineCharacter)
	{
		// None underline
		case 0:
			strcat( font, "\\ulnone" );
			break;

		// Continuous underline
		case 1:
			strcat( font, "\\ul" );
			break;

		// Dotted underline
		case 2:
			strcat( font, "\\uld" );
			break;

		// Dashed underline
		case 3:
			strcat( font, "\\uldash" );
			break;

		// Dash-dotted underline
		case 4:
			strcat( font, "\\uldashd" );
			break;

		// Dash-dot-dotted underline
		case 5:
			strcat( font, "\\uldashdd" );
			break;

		// Double underline
		case 6:
			strcat( font, "\\uldb" );
			break;

		// Heavy wave underline
		case 7:
			strcat( font, "\\ulhwave" );
			break;

		// Long dashed underline
		case 8:
			strcat( font, "\\ulldash" );
			break;

		// Thick underline
		case 9:
			strcat( font, "\\ulth" );
			break;

		// Thick dotted underline
		case 10:
			strcat( font, "\\ulthd" );
			break;

		// Thick dashed underline
		case 11:
			strcat( font, "\\ulthdash" );
			break;

		// Thick dash-dotted underline
		case 12:
			strcat( font, "\\ulthdashd" );
			break;

		// Thick dash-dot-dotted underline
		case 13:
			strcat( font, "\\ulthdashdd" );
			break;

		// Thick long dashed underline
		case 14:
			strcat( font, "\\ulthldash" );
			break;

		// Double wave underline
		case 15:
			strcat( font, "\\ululdbwave" );
			break;

		// Word underline
		case 16:
			strcat( font, "\\ulw" );
			break;

		// Wave underline
		case 17:
			strcat( font, "\\ulwave" );
			break;
	}

	char txt[20] = "";
	// Set paragraph tabbed text
	if ( rtfParFormat.tabbedText == false )
	{
		sprintf( rtfText, "\n%s\\fi%d\\li%d\\ri%d\\sb%d\\sa%d\\sl%d%s %s", text, 
			rtfParFormat.firstLineIndent, rtfParFormat.leftIndent, rtfParFormat.rightIndent, rtfParFormat.spaceBefore, 
			rtfParFormat.spaceAfter, rtfParFormat.lineSpacing, font, rtfParFormat.paragraphText );
	}
	else
		sprintf( rtfText, "\\tab %s", rtfParFormat.paragraphText );

	// Writes RTF paragraph formatting properties
	if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
		result = false;

	// Return error flag
	return result;
}


// Starts new RTF paragraph
int rtf_start_paragraph(char* text, bool newPar)
{
	// Set error flag
	int error = RTF_SUCCESS;

	// Copy paragraph text
	rtfParFormat.paragraphText = new char[strlen(text)];
	strcpy( rtfParFormat.paragraphText, text );

	// Set new paragraph
	rtfParFormat.newParagraph = newPar;

	// Starts new RTF paragraph
	if( !rtf_write_paragraphformat() )
		error = RTF_PARAGRAPHFORMAT_ERROR;

	// Return error flag
	return error;
}


// Gets RTF document formatting properties
RTF_DOCUMENT_FORMAT* rtf_get_documentformat()
{
	// Get current RTF document formatting properties
	return &rtfDocFormat;
}


// Gets RTF section formatting properties
RTF_SECTION_FORMAT* rtf_get_sectionformat()
{
	// Get current RTF section formatting properties
	return &rtfSecFormat;
}


// Gets RTF paragraph formatting properties
RTF_PARAGRAPH_FORMAT* rtf_get_paragraphformat()
{
	// Get current RTF paragraph formatting properties
	return &rtfParFormat;
}


// Loads image from file
int rtf_load_image(char* image, int width, int height)
{
	// Set error flag
	int error = RTF_SUCCESS;

	// Check image type
	bool err = false;
	if ( strstr(image,".bmp") == NULL )
	{
		if ( strstr(image,".jpg") == NULL )
		{
			if (strstr(image,".gif") == NULL )
				err = true;
		}
	}

	// If valid image type
	if ( err == false )
	{
		// Free IPicture object
		if ( rtfPicture != NULL )
		{
			rtfPicture->Release();
			rtfPicture = NULL;
		}

		// Read image file
		int imageFile = _open( image, _O_RDONLY | _O_BINARY );
		struct _stat st;
		_fstat( imageFile, &st );
		DWORD nSize = st.st_size;
		BYTE* pBuff = new BYTE[nSize];
		_read( imageFile, pBuff, nSize );
		// Alocate memory for image data
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
		void* pData = GlobalLock(hGlobal);
		memcpy(pData, pBuff, nSize);
		GlobalUnlock(hGlobal);
		// Load image using OLE
		IStream* pStream = NULL;
		if ( CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK )
		{
			HRESULT hr;
			if ((hr = OleLoadPicture( pStream, nSize, FALSE, IID_IPicture, (LPVOID *)&rtfPicture)) != S_OK)
				error = RTF_IMAGE_ERROR;

			pStream->Release();
		}
		delete []pBuff;
		_close(imageFile);

		// If image is loaded
		if ( rtfPicture != NULL )
		{
			// Calculate image size
			long hmWidth;
			long hmHeight;
			rtfPicture->get_Width(&hmWidth);
			rtfPicture->get_Height(&hmHeight);
			int nWidth	= MulDiv( hmWidth, GetDeviceCaps(GetDC(NULL),LOGPIXELSX), 2540 );
			int nHeight	= MulDiv( hmHeight, GetDeviceCaps(GetDC(NULL),LOGPIXELSY), 2540 );

			// Create metafile;
			HDC hdcMeta = CreateMetaFile(NULL);

			// Render picture to metafile
			rtfPicture->Render( hdcMeta, 0, 0, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, NULL );

			// Close metafile
			HMETAFILE hmf = CloseMetaFile(hdcMeta);

			// Get metafile data
			UINT size = GetMetaFileBitsEx( hmf, 0, NULL );
			BYTE* buffer = new BYTE[size];
			GetMetaFileBitsEx( hmf, size, buffer );
			DeleteMetaFile(hmf);

			// Convert metafile binary data to hexadecimal
			char* str = rtf_bin_hex_convert( buffer, size );
			delete []buffer;

			// Format picture paragraph
			RTF_PARAGRAPH_FORMAT* pf = rtf_get_paragraphformat();
			pf->paragraphText = "";
			rtf_write_paragraphformat();

			// Writes RTF picture data
			char rtfText[1024];
			sprintf( rtfText, "\n{\\pict\\wmetafile8\\picwgoal%d\\pichgoal%d\\picscalex%d\\picscaley%d\n", hmWidth, hmHeight, width, height );
			if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
				error = RTF_IMAGE_ERROR;
			fwrite( str, 1, 2*size, rtfFile );
			strcpy( rtfText, "}" );
			fwrite( rtfText, 1, strlen(rtfText), rtfFile );
		}
	}
	else
	{
		// Writes RTF picture data
		char rtfText[1024];
		strcpy( rtfText, "\n\\par\\pard *** Error! Wrong image format ***\\par" );
		fwrite( rtfText, 1, strlen(rtfText), rtfFile );
	}

	// Return error flag
	return error;
}


// Converts binary data to hex
char* rtf_bin_hex_convert(unsigned char* binary, int size)
{
	char* result = new char[2*size];

	char part1, part2;
	for ( int i=0; i<size; i++ )
	{
		part1 = binary[i] / 16;
		if ( part1 < 10 )
			part1 += 48;
		else
		{
			if ( part1 == 10 )
				part1 = 'a';
			if ( part1 == 11 )
				part1 = 'b';
			if ( part1 == 12 )
				part1 = 'c';
			if ( part1 == 13 )
				part1 = 'd';
			if ( part1 == 14 )
				part1 = 'e';
			if ( part1 == 15 )
				part1 = 'f';
		}

		part2 = binary[i] % 16;
		if ( part2 < 10 )
			part2 += 48;
		else
		{
			if ( part2 == 10 )
				part2 = 'a';
			if ( part2 == 11 )
				part2 = 'b';
			if ( part2 == 12 )
				part2 = 'c';
			if ( part2 == 13 )
				part2 = 'd';
			if ( part2 == 14 )
				part2 = 'e';
			if ( part2 == 15 )
				part2 = 'f';
		}

		result[2*i] = part1;
		result[2*i+1] = part2;
	}

	strcat( result, "\0" );

	return result;
}


// Starts new RTF table row
int rtf_start_tablerow()
{
	// Set error flag
	int error = RTF_SUCCESS;

	char tblrw[] = "";
	// Format table row aligment
	switch (rtfRowFormat.rowAligment)
	{
		// Left align
		case RTF_ROWTEXTALIGN_LEFT:
			strcat( tblrw, "\\trql" );
			break;

		// Center align
		case RTF_ROWTEXTALIGN_CENTER:
			strcat( tblrw, "\\trqc" );
			break;

		// Right align
		case RTF_ROWTEXTALIGN_RIGHT:
			strcat( tblrw, "\\trqr" );
			break;
	}

	// Writes RTF table data
	char rtfText[1024];
	sprintf( rtfText, "\n\\trowd\\trgaph115%s\\trleft%d\\trrh%d\\trpaddb%d\\trpaddfb3\\trpaddl%d\\trpaddfl3\\trpaddr%d\\trpaddfr3\\trpaddt%d\\trpaddft3", 
		tblrw, rtfRowFormat.rowLeftMargin, rtfRowFormat.rowHeight, rtfRowFormat.marginTop, rtfRowFormat.marginBottom, rtfRowFormat.marginLeft, rtfRowFormat.marginRight );
	if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
		error = RTF_TABLE_ERROR;

	// Return error flag
	return error;
}


// Ends RTF table row
int rtf_end_tablerow()
{
	// Set error flag
	int error = RTF_SUCCESS;

	// Writes RTF table data
	char rtfText[1024];
	sprintf( rtfText, "\n\\trgaph115\\row\\pard" );
	if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
		error = RTF_TABLE_ERROR;

	// Return error flag
	return error;
}


// Starts new RTF table cell
int rtf_start_tablecell(int rightMargin)
{
	// Set error flag
	int error = RTF_SUCCESS;

	char tblcla[20];
	// Format table cell text aligment
	switch (rtfCellFormat.textVerticalAligment)
	{
		// Top align
		case RTF_CELLTEXTALIGN_TOP:
			strcpy( tblcla, "\\clvertalt" );
			break;

		// Center align
		case RTF_CELLTEXTALIGN_CENTER:
			strcpy( tblcla, "\\clvertalc" );
			break;

		// Bottom align
		case RTF_CELLTEXTALIGN_BOTTOM:
			strcpy( tblcla, "\\clvertalb" );
			break;
	}

	char tblcld[20];
	// Format table cell text direction
	switch (rtfCellFormat.textDirection)
	{
		// Left to right, top to bottom
		case RTF_CELLTEXTDIRECTION_LRTB:
			strcpy( tblcld, "\\cltxlrtb" );
			break;

		// Right to left, top to bottom
		case RTF_CELLTEXTDIRECTION_RLTB:
			strcpy( tblcld, "\\cltxtbrl" );
			break;

		// Left to right, bottom to top
		case RTF_CELLTEXTDIRECTION_LRBT:
			strcpy( tblcld, "\\cltxbtlr" );
			break;

		// Left to right, top to bottom, vertical
		case RTF_CELLTEXTDIRECTION_LRTBV:
			strcpy( tblcld, "\\cltxlrtbv" );
			break;

		// Right to left, top to bottom, vertical
		case RTF_CELLTEXTDIRECTION_RLTBV:
			strcpy( tblcld, "\\cltxtbrlv" );
			break;
	}

	char tbclbrb[1024]="", tbclbrl[1024]="", tbclbrr[1024]="", tbclbrt[1024]="";
	// Format table cell border
	if ( rtfCellFormat.borderBottom.border == true )
	{
		// Bottom cell border
		char tbclbt[20];
		strcpy( tbclbt, "\\clbrdrb" );

		char* border = rtf_get_bordername(rtfCellFormat.borderBottom.BORDERS.borderType);

		sprintf( tbclbrb, "%s%s\\brdrw%d\\brsp%d\\brdrcf%d", tbclbt, border, rtfCellFormat.borderBottom.BORDERS.borderWidth, 
			rtfCellFormat.borderBottom.BORDERS.borderSpace, rtfCellFormat.borderBottom.BORDERS.borderColor );
	}
	if ( rtfCellFormat.borderLeft.border == true )
	{
		// Left cell border
		char tbclbt[20];
		strcpy( tbclbt, "\\clbrdrl" );

		char* border = rtf_get_bordername(rtfCellFormat.borderLeft.BORDERS.borderType);

		sprintf( tbclbrl, "%s%s\\brdrw%d\\brsp%d\\brdrcf%d", tbclbt, border, rtfCellFormat.borderLeft.BORDERS.borderWidth, 
		rtfCellFormat.borderLeft.BORDERS.borderSpace, rtfCellFormat.borderLeft.BORDERS.borderColor );
	}
	if ( rtfCellFormat.borderRight.border == true )
	{
		// Right cell border
		char tbclbt[20];
		strcpy( tbclbt, "\\clbrdrr" );

		char* border = rtf_get_bordername(rtfCellFormat.borderRight.BORDERS.borderType);

		sprintf( tbclbrr, "%s%s\\brdrw%d\\brsp%d\\brdrcf%d", tbclbt, border, rtfCellFormat.borderRight.BORDERS.borderWidth, 
		rtfCellFormat.borderRight.BORDERS.borderSpace, rtfCellFormat.borderRight.BORDERS.borderColor );
	}
	if ( rtfCellFormat.borderTop.border == true )
	{
		// Top cell border
		char tbclbt[20];
		strcpy( tbclbt, "\\clbrdrt" );

		char* border = rtf_get_bordername(rtfCellFormat.borderTop.BORDERS.borderType);

		sprintf( tbclbrt, "%s%s\\brdrw%d\\brsp%d\\brdrcf%d", tbclbt, border, rtfCellFormat.borderTop.BORDERS.borderWidth, 
		rtfCellFormat.borderTop.BORDERS.borderSpace, rtfCellFormat.borderTop.BORDERS.borderColor );
	}

	// Format table cell shading
	char shading[100] = "";
	if ( rtfCellFormat.cellShading == true )
	{
		char* sh = rtf_get_shadingname( rtfCellFormat.SHADING.shadingType, true );

		// Set paragraph shading color
		sprintf( shading, "%s\\clshdgn%d\\clcfpat%d\\clcbpat%d", sh, rtfCellFormat.SHADING.shadingIntensity, rtfCellFormat.SHADING.shadingFillColor, rtfCellFormat.SHADING.shadingBkColor );
	}

	// Writes RTF table data
	char rtfText[1024];
	sprintf( rtfText, "\n\\tcelld%s%s%s%s%s%s%s\\cellx%d", tblcla, tblcld, tbclbrb, tbclbrl, tbclbrr, tbclbrt, shading, rightMargin );
	if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
		error = RTF_TABLE_ERROR;

	// Return error flag
	return error;
}


// Ends RTF table cell
int rtf_end_tablecell()
{
	// Set error flag
	int error = RTF_SUCCESS;

	// Writes RTF table data
	char rtfText[1024];
	strcpy( rtfText, "\n\\cell " );
	if ( fwrite( rtfText, 1, strlen(rtfText), rtfFile ) < strlen(rtfText) )
		error = RTF_TABLE_ERROR;

	// Return error flag
	return error;
}


// Gets RTF table row formatting properties
RTF_TABLEROW_FORMAT* rtf_get_tablerowformat()
{
	// Get current RTF table row formatting properties
	return &rtfRowFormat;
}


// Sets RTF table row formatting properties
void rtf_set_tablerowformat(RTF_TABLEROW_FORMAT* rf)
{
	// Set new RTF table row formatting properties
	memcpy( &rtfRowFormat, rf, sizeof(RTF_TABLEROW_FORMAT) );
}


// Gets RTF table cell formatting properties
RTF_TABLECELL_FORMAT* rtf_get_tablecellformat()
{
	// Get current RTF table cell formatting properties
	return &rtfCellFormat;
}


// Sets RTF table cell formatting properties
void rtf_set_tablecellformat(RTF_TABLECELL_FORMAT* cf)
{
	// Set new RTF table cell formatting properties
	memcpy( &rtfCellFormat, cf, sizeof(RTF_TABLECELL_FORMAT) );
}


// Gets border name
char* rtf_get_bordername(int border_type)
{
	char* border = new char[20];

	switch (border_type)
	{
		// Single-thickness border
		case RTF_PARAGRAPHBORDERTYPE_STHICK:
			strcpy( border, "\\brdrs" );
			break;

		// Double-thickness border
		case RTF_PARAGRAPHBORDERTYPE_DTHICK:
			strcpy( border, "\\brdrth" );
			break;

		// Shadowed border
		case RTF_PARAGRAPHBORDERTYPE_SHADOW:
			strcpy( border, "\\brdrsh" );
			break;

		// Double border
		case RTF_PARAGRAPHBORDERTYPE_DOUBLE:
			strcpy( border, "\\brdrdb" );
			break;

		// Dotted border
		case RTF_PARAGRAPHBORDERTYPE_DOT:
			strcpy( border, "\\brdrdot" );
			break;

		// Dashed border
		case RTF_PARAGRAPHBORDERTYPE_DASH:
			strcpy( border, "\\brdrdash" );
			break;

		// Hairline border
		case RTF_PARAGRAPHBORDERTYPE_HAIRLINE:
			strcpy( border, "\\brdrhair" );
			break;

		// Inset border
		case RTF_PARAGRAPHBORDERTYPE_INSET:
			strcpy( border, "\\brdrinset" );
			break;

		// Dashed border (small)
		case RTF_PARAGRAPHBORDERTYPE_SDASH:
			strcpy( border, "\\brdrdashsm" );
			break;

		// Dot-dashed border
		case RTF_PARAGRAPHBORDERTYPE_DOTDASH:
			strcpy( border, "\\brdrdashd" );
			break;

		// Dot-dot-dashed border
		case RTF_PARAGRAPHBORDERTYPE_DOTDOTDASH:
			strcpy( border, "\\brdrdashdd" );
			break;

		// Outset border
		case RTF_PARAGRAPHBORDERTYPE_OUTSET:
			strcpy( border, "\\brdroutset" );
			break;

		// Triple border
		case RTF_PARAGRAPHBORDERTYPE_TRIPLE:
			strcpy( border, "\\brdrtriple" );
			break;

		// Wavy border
		case RTF_PARAGRAPHBORDERTYPE_WAVY:
			strcpy( border, "\\brdrwavy" );
			break;

		// Double wavy border
		case RTF_PARAGRAPHBORDERTYPE_DWAVY:
			strcpy( border, "\\brdrwavydb" );
			break;

		// Striped border
		case RTF_PARAGRAPHBORDERTYPE_STRIPED:
			strcpy( border, "\\brdrdashdotstr" );
			break;

		// Embossed border
		case RTF_PARAGRAPHBORDERTYPE_EMBOSS:
			strcpy( border, "\\brdremboss" );
			break;

		// Engraved border
		case RTF_PARAGRAPHBORDERTYPE_ENGRAVE:
			strcpy( border, "\\brdrengrave" );
			break;
	}

	return border;
}


// Gets shading name
char* rtf_get_shadingname(int shading_type, bool cell)
{
	char* shading = new char[20];

	if ( cell == false )
	{
		switch (shading_type)
		{
			// Fill shading
			case RTF_PARAGRAPHSHADINGTYPE_FILL:
				strcpy( shading, "" );
				break;

			// Horizontal background pattern
			case RTF_PARAGRAPHSHADINGTYPE_HORIZ:
				strcpy( shading, "\\bghoriz" );
				break;

			// Vertical background pattern
			case RTF_PARAGRAPHSHADINGTYPE_VERT:
				strcpy( shading, "\\bgvert" );
				break;

			// Forward diagonal background pattern
			case RTF_PARAGRAPHSHADINGTYPE_FDIAG:
				strcpy( shading, "\\bgfdiag" );
				break;

			// Backward diagonal background pattern
			case RTF_PARAGRAPHSHADINGTYPE_BDIAG:
				strcpy( shading, "\\bgbdiag" );
				break;

			// Cross background pattern
			case RTF_PARAGRAPHSHADINGTYPE_CROSS:
				strcpy( shading, "\\bgcross" );
				break;

			// Diagonal cross background pattern
			case RTF_PARAGRAPHSHADINGTYPE_CROSSD:
				strcpy( shading, "\\bgdcross" );
				break;

			// Dark horizontal background pattern
			case RTF_PARAGRAPHSHADINGTYPE_DHORIZ:
				strcpy( shading, "\\bgdkhoriz" );
				break;

			// Dark vertical background pattern
			case RTF_PARAGRAPHSHADINGTYPE_DVERT:
				strcpy( shading, "\\bgdkvert" );
				break;

			// Dark forward diagonal background pattern
			case RTF_PARAGRAPHSHADINGTYPE_DFDIAG:
				strcpy( shading, "\\bgdkfdiag" );
				break;

			// Dark backward diagonal background pattern
			case RTF_PARAGRAPHSHADINGTYPE_DBDIAG:
				strcpy( shading, "\\bgdkbdiag" );
				break;

			// Dark cross background pattern
			case RTF_PARAGRAPHSHADINGTYPE_DCROSS:
				strcpy( shading, "\\bgdkcross" );
				break;

			// Dark diagonal cross background pattern
			case RTF_PARAGRAPHSHADINGTYPE_DCROSSD:
				strcpy( shading, "\\bgdkdcross" );
				break;
		}
	}
	else
	{
		switch (shading_type)
		{
			// Fill shading
			case RTF_CELLSHADINGTYPE_FILL:
				strcpy( shading, "" );
				break;

			// Horizontal background pattern
			case RTF_CELLSHADINGTYPE_HORIZ:
				strcpy( shading, "\\clbghoriz" );
				break;

			// Vertical background pattern
			case RTF_CELLSHADINGTYPE_VERT:
				strcpy( shading, "\\clbgvert" );
				break;

			// Forward diagonal background pattern
			case RTF_CELLSHADINGTYPE_FDIAG:
				strcpy( shading, "\\clbgfdiag" );
				break;

			// Backward diagonal background pattern
			case RTF_CELLSHADINGTYPE_BDIAG:
				strcpy( shading, "\\clbgbdiag" );
				break;

			// Cross background pattern
			case RTF_CELLSHADINGTYPE_CROSS:
				strcpy( shading, "\\clbgcross" );
				break;

			// Diagonal cross background pattern
			case RTF_CELLSHADINGTYPE_CROSSD:
				strcpy( shading, "\\clbgdcross" );
				break;

			// Dark horizontal background pattern
			case RTF_CELLSHADINGTYPE_DHORIZ:
				strcpy( shading, "\\clbgdkhoriz" );
				break;

			// Dark vertical background pattern
			case RTF_CELLSHADINGTYPE_DVERT:
				strcpy( shading, "\\clbgdkvert" );
				break;

			// Dark forward diagonal background pattern
			case RTF_CELLSHADINGTYPE_DFDIAG:
				strcpy( shading, "\\clbgdkfdiag" );
				break;

			// Dark backward diagonal background pattern
			case RTF_CELLSHADINGTYPE_DBDIAG:
				strcpy( shading, "\\clbgdkbdiag" );
				break;

			// Dark cross background pattern
			case RTF_CELLSHADINGTYPE_DCROSS:
				strcpy( shading, "\\clbgdkcross" );
				break;

			// Dark diagonal cross background pattern
			case RTF_CELLSHADINGTYPE_DCROSSD:
				strcpy( shading, "\\clbgdkdcross" );
				break;
		}
	}

	return shading;
}
