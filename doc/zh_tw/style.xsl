<?xml version='1.0'?>
<xsl:stylesheet  
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<!-- Please modify this path to correct docbook stylesheet-->
<xsl:import href="/usr/share/sgml/docbook/xsl-stylesheets/html/chunk.xsl"/>
<xsl:param name="l10n.xml" select="document('../translations/l10n.xml')"/>
<xsl:template name="person.name.family-given">
  <xsl:param name="node" select="."/>

  <!-- The family-given style applies a convention for identifying given -->
  <!-- and family names in locales where it may be ambiguous -->
  <xsl:apply-templates select="$node//surname[1]"/>

  <xsl:if test="$node//surname and $node//firstname">
    <xsl:text> </xsl:text>
  </xsl:if>

  <xsl:apply-templates select="$node//firstname[1]"/>
</xsl:template>
</xsl:stylesheet>
