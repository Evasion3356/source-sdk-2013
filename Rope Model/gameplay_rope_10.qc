$modelname "props_shared/ropes/gameplay_rope_10"

$cdmaterials "props_shared/ropes"

$surfaceprop "paper"

$origin 0.000000 0.000000 0.000000 -90.000000

$scale 1.000000

$body "gameplay_rope_10" "gameplay_rope_10.SMD"

$collisionmodel "gameplay_rope_10_phys.SMD" {
    $concave
    $automass
    $maxconvexpieces 10000
}

$sequence "idle" "anims/idle.SMD"

$texturegroup "skinfamilies"
{
    { gameplay_ropes01a }
    { gameplay_ropes01b }
}
