// #ifndef RAYGUI_IMPLEMENTATION
//
// #pragma message("DEPENDS ON RAYGUI_IMPLEMENTATION in same .C file")
//
// #else 
#include <stdint.h>
typedef struct 
{
	const char *name;
	int					nKeys;
	float				*keys;
} track_t;

typedef struct
{
	float frame;
	float minValue;
	float maxValue;
	Rectangle bounds;
	int		nTracks;
	track_t *tracks;
	bool	isPlaying;
	bool	isEditingFrameSlider;
	bool	isDropdownActive;
	int 	selected;
	int		cellHeight;
	Rectangle dropDownRect;

} timeline_t;

#ifdef _TIMELINE_IMPL_ 

static float sliderPos;

static float GuiSliderProOwning(Rectangle bounds, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue, int sliderWidth, bool editMode)
{
    GuiState state = (GuiState)GuiGetState();

    float tempValue = *value;
    bool pressed = false;

    int sliderValue = (int64_t)(((tempValue - minValue)/(maxValue - minValue))*(bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH)));

    Rectangle slider = { 
        bounds.x, 
        bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH) + GuiGetStyle(SLIDER, SLIDER_PADDING),       
        0, 
        bounds.height - 2*GuiGetStyle(SLIDER, BORDER_WIDTH) - 2*GuiGetStyle(SLIDER, SLIDER_PADDING)
    };

    if (sliderWidth > 0)        // Slider
    {
        slider.x += (sliderValue - sliderWidth/2);
        slider.width = (float)sliderWidth;
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        slider.x += GuiGetStyle(SLIDER, BORDER_WIDTH);
        slider.width = (float)sliderValue;
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && (editMode || !guiLocked))
    {
        Vector2 mousePoint = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                pressed = true;
            }
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && editMode)
        {
            pressed = true;
        }
        if (editMode)
        {
            state = STATE_PRESSED;
            tempValue = ((maxValue - minValue)*(mousePoint.x - (float)(bounds.x + sliderWidth/2)))/(float)(bounds.width - sliderWidth) + minValue;

            if (sliderWidth > 0) slider.x = mousePoint.x - slider.width/2;  // Slider
            else if (sliderWidth == 0) slider.width = (float)sliderValue;          // SliderBar
            
        }
        else if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = STATE_FOCUSED;
        }

        if (tempValue > maxValue) tempValue = maxValue;
        else if (tempValue < minValue) tempValue = minValue;
    }


    // Bar limits check
    if (sliderWidth > 0)        // Slider
    {
        if (slider.x <= (bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH))) slider.x = bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH);
        else if ((slider.x + slider.width) >= (bounds.x + bounds.width)) slider.x = bounds.x + bounds.width - slider.width - GuiGetStyle(SLIDER, BORDER_WIDTH);
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        if (slider.width > bounds.width) slider.width = bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH);
    }

    //--------------------------------------------------------------------
    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(SLIDER, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(SLIDER, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(SLIDER, (state != STATE_DISABLED)?  BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha));

    // Draw slider internal bar (depends on state)
    if ((state == STATE_NORMAL) || (state == STATE_PRESSED)) 
        GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)), guiAlpha));
    else if (state == STATE_FOCUSED) 
        GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, TEXT_COLOR_FOCUSED)), guiAlpha));

		sliderPos = slider.x;
    // Draw left/right text if provided
    if (textLeft != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = (float)GetTextWidth(textLeft);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x - textBounds.width - GuiGetStyle(SLIDER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textLeft, textBounds, TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }

    if (textRight != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = (float)GetTextWidth(textRight);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(SLIDER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textRight, textBounds, TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }
    //--------------------------------------------------------------------
    
    *value = tempValue;
    return pressed;
}

static bool GuiSliderOwning(Rectangle bounds, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue, bool editMode)
{
    return GuiSliderProOwning(bounds, textLeft, textRight, value, minValue, maxValue, GuiGetStyle(SLIDER, SLIDER_WIDTH), editMode);
}


#define TEXTBLOCK 96

void TimelineTrack(timeline_t *timeline,const char *label,Rectangle bounds,float *keys,int kcount)
{
	//	
	int sliderWidth = GuiGetStyle(SLIDER, SLIDER_WIDTH);
	Vector2 mousePoint = GetMousePosition();

	//	draw label 
	GuiDisableTooltip();

	if (GuiButton((Rectangle){ bounds.x, bounds.y, timeline->cellHeight, timeline->cellHeight }, GuiIconText(ICON_LOCK_OPEN, "")))
	{
	}
	if (GuiButton((Rectangle){ bounds.x+timeline->cellHeight, bounds.y, timeline->cellHeight, timeline->cellHeight }, GuiIconText(ICON_PLAYER_PLAY, "")))
	{
	}

	GuiEnableTooltip();
	GuiLabel((Rectangle){bounds.x+(timeline->cellHeight*2),bounds.y,GetTextWidth(label),timeline->cellHeight},label);
	//	if we're not pressing buttons then selected is reset 
	if ((!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) && (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT)))
		timeline->selected = -1;
	//	if mouse over 
	if (CheckCollisionPointRec(mousePoint, bounds))
	{
//		GuiDrawRectangle(bounds, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, BASE_COLOR_FOCUSED)), guiAlpha));
	}
	//	draw the keys
	bounds.x+=TEXTBLOCK;
	bounds.width-=TEXTBLOCK;
	int over = -1;
	for (int q=0;q<kcount;q++) 
	{
		float value;
		GuiState state = GuiGetState();
;
		value = keys[q];

		//	slidervalue for this keyframe
    float sliderValue = (float)(((value - timeline->minValue)/(timeline->maxValue - timeline->minValue))*(bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH)));
		//	get rect
    Rectangle slider = { bounds.x, bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH) + GuiGetStyle(SLIDER, SLIDER_PADDING),
                         0, bounds.height - 2.0f*GuiGetStyle(SLIDER, BORDER_WIDTH) - 2.0f*GuiGetStyle(SLIDER, SLIDER_PADDING) };
		//	set normal color 
		Color circle_color = GetColor(GuiGetStyle(SLIDER, TEXT_COLOR_NORMAL));
		if (timeline->selected==(int64_t)keys+q)
		{
			//	move value based on movement
			value = ((timeline->maxValue - timeline->minValue)*(mousePoint.x - (float)(bounds.x + (float)sliderWidth/2.0f)))/(float)(bounds.width - sliderWidth) + timeline->minValue;

			if (value > timeline->maxValue) value = timeline->maxValue;
			else if (value < timeline->minValue) value = timeline->minValue;
			//	change to red
			circle_color=RED;
			//	set the key to new value 
			keys[q]=value;
		}

    sliderValue = (float)(((value - timeline->minValue)/(timeline->maxValue - timeline->minValue))*(bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH)));

    if (sliderWidth > 0)        // Slider
    {
        slider.x += (sliderValue - sliderWidth/2);
        slider.width = (float)sliderWidth;
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        slider.x += GuiGetStyle(SLIDER, BORDER_WIDTH);
        slider.width = (float)sliderValue;
    }

		float offsetx = slider.width/2.0f;
		float offsety = slider.height/2.0f;
		//	if we can edit
    if ((state != STATE_DISABLED) && !guiLocked && timeline->isEditingFrameSlider==false)
    {
			if (CheckCollisionPointRec(mousePoint, slider))
			{
				state=STATE_FOCUSED;
				over = (int64_t)keys+q;
				//	if nothing selected then select this one if mouse pressed
				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
				{
					if (timeline->selected==-1)
						timeline->selected = (int64_t)keys+q;
				}
				//	drop down if wanted
				if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
				{
					timeline->dropDownRect=(Rectangle){ mousePoint.x, mousePoint.y, 125, 32 };
					timeline->isDropdownActive=true;
				}
			}
		}
		//	if we're over or have a selection for this key show the tooltip
		if ((over==(int64_t)keys+q) || (timeline->selected==(int64_t)keys+q))
		{
			GuiSetTooltip(TextFormat("%d",(int64_t)value));
			GuiTooltip(slider);
		}
		//	draw the key
		DrawCircle(slider.x+offsetx,slider.y+offsety,slider.height/2,Fade(circle_color, guiAlpha));
	}
}


void Timeline(timeline_t *timeline)
{
	float sy = 0;
	static int scroll = 0;
	float step = 0;
	int y = timeline->bounds.y;
	Rectangle *block =&timeline->bounds;

	GuiDisableTooltip();
	timeline->bounds.height=((5+timeline->nTracks)*timeline->cellHeight);
	GuiDrawRectangle(timeline->bounds,0,BLACK,(Color){0,0,0,230});
	int bs = timeline->cellHeight*2;
	//	draw control buttons
	if (GuiButton((Rectangle){ block->x+(0*bs), block->y, bs, bs }, GuiIconText(ICON_PLAYER_PREVIOUS, "")))
	{
	}
	if (GuiButton((Rectangle){ block->x+(1*bs), block->y, bs, bs }, GuiIconText(ICON_PLAYER_PLAY_BACK, "")))
	{
	}
	if (GuiButton((Rectangle){ block->x+(2*bs), block->y, bs, bs }, GuiIconText(timeline->isPlaying ? ICON_PLAYER_PAUSE:ICON_PLAYER_PLAY , "")))
	{
		timeline->isPlaying=!timeline->isPlaying;
	}
	if (GuiButton((Rectangle){ block->x+(3*bs), block->y, bs, bs }, GuiIconText(ICON_PLAYER_NEXT, "")))
	{
	}
	if (GuiButton((Rectangle){ block->x+(4*bs), block->y, bs, bs }, GuiIconText(ICON_HELP, "")))
	{
	}

	GuiEnableTooltip();
	float scale = timeline->maxValue/((block->width-TEXTBLOCK)/32.0f);
	step = (float)scroll;

	if (timeline->isPlaying==true)
	{
		timeline->frame+=1;
	}
	y+=bs;
	timeline->frame = fmod(timeline->frame,timeline->maxValue+1.0f);
	if (GuiSliderOwning((Rectangle){ block->x+TEXTBLOCK, y, block->width-TEXTBLOCK, timeline->cellHeight }, NULL, NULL, &timeline->frame, timeline->minValue, timeline->maxValue, timeline->isEditingFrameSlider)) 
	{
		timeline->isEditingFrameSlider = !timeline->isEditingFrameSlider;
	}

	for (int x=TEXTBLOCK;x<block->width;x+=32)
	{
		DrawLine((int64_t)block->x+x,y,(int64_t)block->x+x,y+bs,WHITE);
		GuiDrawText(TextFormat("%03d",(int64_t)step), (Rectangle){block->x+x,y+timeline->cellHeight,32,timeline->cellHeight},
                TEXT_ALIGN_LEFT, WHITE);
		step+=scale;
	}	
	float rpos;
	rpos = sliderPos;

	y+=timeline->cellHeight*2;
	int slidery=y+timeline->cellHeight;
	GuiLabel((Rectangle){block->x,y,block->width,timeline->cellHeight},TextFormat("%d",(int64_t)timeline->frame));
	y+=timeline->cellHeight;

	for (int q=0;q<timeline->nTracks;q++)
	{
		if (((int64_t)q)&1)
			DrawRectangle(block->x+TEXTBLOCK,y,block->width-TEXTBLOCK,timeline->cellHeight,(Color){96,96,96,128});
		else 
			DrawRectangle(block->x+TEXTBLOCK,y,block->width-TEXTBLOCK,timeline->cellHeight,(Color){64,64,64,128});

		TimelineTrack(timeline,timeline->tracks[q].name,(Rectangle){block->x,y,block->width,timeline->cellHeight},&timeline->tracks[q].keys[0],timeline->tracks[q].nKeys);
		y+=timeline->cellHeight;
	}
	DrawLine(rpos+4,slidery,rpos+4,y,RED);
//	scroll = GuiSlider((Rectangle){block->x,y+12,block->width,12},scroll,0,255,GuiGetStyle(SLIDER, SLIDER_WIDTH),1.0f);

	if (timeline->dropDownRect.x!=-1)
	{
		GuiDisableTooltip();
		if (GuiButton(timeline->dropDownRect,"Delete"))
		{

//		}
//		if (GuiDropdownBox(timeline->dropDownRect, "Step;Lerp;Delete", &dropdownBox000Active, timeline->isDropdownActive))
//		{
			timeline->dropDownRect.x=-1;
			timeline->isDropdownActive=false;
		}
		GuiEnableTooltip();
	}
}

#else 
void Timeline(timeline_t *timeline);
#endif

// #endif
