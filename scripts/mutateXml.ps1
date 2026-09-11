param(
  [Parameter(
    Mandatory = $true,
    HelpMessage = "Path to the xml file to mutate"
  )]
  [alias("p","path")]
  [string]$filePath,
  [Parameter(
    Mandatory = $true,
    HelpMessage = "XPath to mutate"
  )]
  [alias("xp","xpath")]
  [string]$_XPath,
  [Parameter(
    HelpMessage = "Name of new element to create"
  )]
  [alias("newElem")]
  [string]$newElemName,
  [Parameter(
    HelpMessage = "Value for mutated/created element"
  )]
  [alias("val","value")]
  [string]$elemValue,
  [Parameter(
    HelpMessage = "Attributes for mutated/created element"
  )]
  [alias("attributes","attribs")]
  [hashtable]$_attribs
)

[xml]$xml = Get-Content -Path $filepath
$nsMgr = $null
if ($xml.DocumentElement.NamespaceURI) {
  $nsMgr = New-Object System.Xml.XmlNamespaceManager($xml.NameTable)
  $nsMgr.AddNamespace("ms", $xml.DocumentElement.NamespaceURI)
}

$targetNode = if ($nsMgr) {
  $xml.SelectSingleNode($_XPath, $nsMgr)
} else {
  $xml.SelectSingleNode($_XPath)
}

if (-not $targetNode) {
  throw "Target node not found for XPath $XPath"
}

if (-not $newElemName) {
  $targetNode.InnerText = $elemValue
} else {
  $nsUri = $targetNode.NamespaceURI
  $newElem = if ($nsUri) {
    $xml.CreateElement($newElemName, $nsUri)
  } else {
    $xml.CreateElement($newElemName)
  }
  if ($elemValue) {
    $newElem.InnerText = $elemValue
  }
  if ($_attribs) {
    foreach ($key in $_attribs.keys) {
      $newElem.SetAttribute($key, $_attribs[$key])
    }
  }
  [void]$targetNode.AppendChild($newElem)
}

$xml.Save($filePath)
