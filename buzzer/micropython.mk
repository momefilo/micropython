
BUZZER_SOUND_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(BUZZER_SOUND_MOD_DIR)/buzzer_sound.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(BUZZER_SOUND_MOD_DIR)
