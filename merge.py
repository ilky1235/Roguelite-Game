import os

# 1. The exact dependency order required for your game to compile
files_in_order = [
    "dynamic_array.h", 
    "entity.h", 
    "projectile.h", 
    "player.h", 
    "enemy.h", 
    "room.h", 
    "level_manager.h", 
    "GameController.h",
    "entity.cpp", 
    "projectile.cpp", 
    "player.cpp", 
    "enemy.cpp", 
    "room.cpp", 
    "level_manager.cpp", 
    "GameController.cpp", 
    "main.cpp"
]

output_filename = "combined_game.cpp"

with open(output_filename, "w") as outfile:
    outfile.write("// Combined SplashKit Game File\n\n")
    
    for fname in files_in_order:
        if not os.path.exists(fname):
            print(f"Warning: Could not find {fname}, skipping...")
            continue
            
        outfile.write(f"\n// ==========================================\n")
        outfile.write(f"// --- START OF {fname} ---\n")
        outfile.write(f"// ==========================================\n\n")
        
        with open(fname, "r") as infile:
            for line in infile:
                # 2. Automatically delete local includes, but KEEP system includes like <cmath>
                if line.strip().startswith('#include "') and "splashkit.h" not in line:
                    outfile.write(f"// Removed local include: {line.strip()}\n")
                else:
                    outfile.write(line)
                    
        outfile.write(f"\n// --- END OF {fname} ---\n\n")

print(f"Success! All files cleanly merged into '{output_filename}'.")